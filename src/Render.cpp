#include "Render.h"

#define GLSLify(version, shader) "#version " #version "\n" #shader

HRESULT fillBMP(std::string name)
{
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int width, height;
    unsigned int imageSize;
    char* data = 0;

    // Open the file
    FILE * file = fopen(name.c_str(), "rb");
    if (!file)
        return E_FAIL;
    if (fread(header, 1, 54, file) != 54)
        return E_FAIL;

    if (header[0] != 'B' || header[1] != 'M')
        return E_FAIL;

    // Read ints from the byte array
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    if (imageSize == 0)    imageSize = width*height * 3;    // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)      dataPos = 54;                    // The BMP header is done that way
    data = new char[imageSize];
    if (!data)
        return E_FAIL;
    fread(data, 1, imageSize, file);
    fclose(file);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    delete[] data;

    return S_OK;
}

GLuint aa::render::CreateTexture2D(char const* filename)
{
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (fillBMP(filename) != S_OK)
        return 0;
    return tex;
}

struct TexturedQuad
{
    GLuint vao, vbo, program;
    GLint uloc_tex, uloc_res, uloc_dim;

    TexturedQuad()
    {
        // init shaders
        program = glCreateProgram();
        {
            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            static const char* vs_shdr = GLSLify(330,
                in vec2 iPos;
                uniform vec2 uRes;
                uniform vec4 uDim;
                out vec2 uv;

                void main()
                {
                    vec2 sz = 2.0 * (uDim.zw / uRes);
                    vec2 disp = 2.0 * (uDim.xy / uRes);
                    vec2 p = iPos * 0.5 + vec2(0.5, 0.5);
                    p *= sz;
                    p += disp - vec2(1, 1);
                    gl_Position = vec4(p.x, -p.y, 0.5, 1.0);

                    uv = (iPos + vec2(1.0)) * 0.5;
                }
            );
            glShaderSource(vs, 1, &vs_shdr, NULL);
            glCompileShader(vs);
            glAttachShader(program, vs);

            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fs_shdr = GLSLify(330,
                in vec2 uv;
                uniform sampler2D uTex;
                out vec4 fragColor;

                void main()
                {
                    fragColor = texture(uTex, uv);
                    //fragColor = vec4(uv, 0.0, 1.0);
                }
            );
            glShaderSource(fs, 1, &fs_shdr, NULL);
            glCompileShader(fs);
            glAttachShader(program, fs);

            glLinkProgram(program);
            glDeleteShader(vs);
            glDeleteShader(fs);
            {
                int infologLen = 0;
                int charsWritten = 0;
                GLchar *infoLog = NULL;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);
                if (infologLen > 0)
                {
                    infoLog = (GLchar*)malloc(infologLen);
                    if (infoLog == NULL)
                        return;
                    glGetProgramInfoLog(program, infologLen, &charsWritten, infoLog);
                }
                printf("%s", infoLog);
                delete[] infoLog;
            }
        }
        uloc_res = glGetUniformLocation(program, "uRes");
        uloc_tex = glGetUniformLocation(program, "uTex");
        uloc_dim = glGetUniformLocation(program, "uDim");

        // init fs quad
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        GLfloat verts[12] = { -1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1 };
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindVertexArray(0);
    }

    void Draw(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height)
    {
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(uloc_tex, 0);
        RECT hrect;
        GetClientRect(aa::window::g_hWnd, &hrect);
        glUniform2f(uloc_res, (float)hrect.right, (float)hrect.bottom);
        glUniform4f(uloc_dim, (float)x, (float)y, (float)width, (float)height);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    ~TexturedQuad()
    {
        glBindVertexArray(0);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        vbo = 0;
        vao = 0;

        glDeleteProgram(program);
        program = 0;
    }
};

void aa::render::DrawTexturedQuad(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height)
{
    static TexturedQuad qr;
    qr.Draw(texture, x, y, width, height);
}

struct TerrainLOD
{
    GLuint program, vao, vbo;
    GLuint uloc_heightmap, uloc_m, uloc_v, uloc_p;
    static const char patchCount = 8;

    TerrainLOD()
    {
        // init shaders
        program = glCreateProgram();
        {
            // vertex shader
            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            static const char* vs_shdr = GLSLify(400,
                in vec2 position;
                uniform sampler2D uHeightmap;

                void main()
                {
                    float height = texture(uHeightmap, position).r;
                    gl_Position = vec4(position.x, position.y, height, 1.0);
                }
            );
            glShaderSource(vs, 1, &vs_shdr, NULL);
            glCompileShader(vs);
            glAttachShader(program, vs);

            // tessellation control shader
            GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
            const char* tcs_shdr = GLSLify(400,
                layout(vertices = 4) out;

                void main()
                {
                    if (gl_InvocationID == 0)
                    {
                        gl_TessLevelInner[0] = 1.0f;
                        gl_TessLevelInner[1] = 1.0f;

                        gl_TessLevelOuter[0] = 1.0f;
                        gl_TessLevelOuter[1] = 1.0f;
                        gl_TessLevelOuter[2] = 1.0f;
                        gl_TessLevelOuter[3] = 1.0f;
                    }
                    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
                }
            );
            glShaderSource(tcs, 1, &tcs_shdr, NULL);
            glCompileShader(tcs);
            glAttachShader(program, tcs);

            // tessellation evaluation shader
            GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
            const char* tes_shdr = GLSLify(400,
                layout(quads, fractional_odd_spacing, ccw) in;
                out vec2 uv;

                uniform sampler2D uHeightmap;
                uniform mat4 uModelMat;
                uniform mat4 uViewMat;
                uniform mat4 uProjMat;

                void main(){
                    float u = gl_TessCoord.x;
                    float v = gl_TessCoord.y;

                    vec4 a = mix(gl_in[1].gl_Position, gl_in[0].gl_Position, u);
                    vec4 b = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, u);
                    vec4 pos = mix(a, b, v);
                    uv = pos.xy;
                    float height = texture(uHeightmap, uv).r;
                    gl_Position = uModelMat * uViewMat * uProjMat * vec4(uv, height, 1.0);
                }
            );
            glShaderSource(tes, 1, &tes_shdr, NULL);
            glCompileShader(tes);
            glAttachShader(program, tes);

            // fragment shader
            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fs_shdr = GLSLify(400,
                in vec2 uv;
                uniform sampler2D uHeightmap;
                out vec4 fragColor;

                void main()
                {
                    float h = texture(uHeightmap, uv).r;
                    fragColor = vec4(uv.x, uv.y, 0.5, 1.0);
                }
            );
            glShaderSource(fs, 1, &fs_shdr, NULL);
            glCompileShader(fs);
            glAttachShader(program, fs);

            glLinkProgram(program);
            glDeleteShader(vs);
            glDeleteShader(tcs);
            glDeleteShader(tes);
            glDeleteShader(fs);
            {
                int infologLen = 0;
                int charsWritten = 0;
                GLchar *infoLog = NULL;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);
                if (infologLen > 0)
                {
                    infoLog = (GLchar*)malloc(infologLen);
                    if (infoLog == NULL)
                        return;
                    glGetProgramInfoLog(program, infologLen, &charsWritten, infoLog);
                }
                printf("%s", infoLog);
                delete[] infoLog;
            }
        }
        uloc_heightmap = glGetUniformLocation(program, "uHeightmap");
        uloc_m = glGetUniformLocation(program, "uModelMat");
        uloc_v = glGetUniformLocation(program, "uViewMat");
        uloc_p = glGetUniformLocation(program, "uProjMat");

        // init patches
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        GLfloat verts[patchCount][patchCount][4][2];
        float d = 1.0f / (float)patchCount;
        for (char i = 0; i < patchCount; ++i)
        {
            for (char j = 0; j < patchCount; ++j)
            {
                verts[i][j][0][0] = i          * d;
                verts[i][j][0][1] = j          * d;

                verts[i][j][1][0] = (i + 1)    * d;
                verts[i][j][1][1] = j          * d;

                verts[i][j][2][0] = (i + 1)    * d;
                verts[i][j][2][1] = (j + 1)    * d;

                verts[i][j][3][0] = i          * d;
                verts[i][j][3][1] = (j + 1)    * d;
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glBindVertexArray(0);
    }

    ~TerrainLOD()
    {
        glBindVertexArray(0);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        vbo = 0;
        vao = 0;

        glDeleteProgram(program);
        program = 0;
    }

    void Draw(GLuint heightmap, glm::mat4 m, glm::mat4 v, glm::mat4 p)
    {
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightmap);
        glUniform1i(uloc_heightmap, 0);
        glUniformMatrix4fv(uloc_m, 1, GL_FALSE, glm::value_ptr(m));
        glUniformMatrix4fv(uloc_v, 1, GL_FALSE, glm::value_ptr(v));
        glUniformMatrix4fv(uloc_p, 1, GL_FALSE, glm::value_ptr(p));

        glBindVertexArray(vao);
        glDrawArrays(GL_PATCHES, 0, patchCount*patchCount * 4);
    }
};

void aa::render::DrawLODTerrain(GLuint heightmap, glm::mat4 m, glm::mat4 v, glm::mat4 p)
{
    static TerrainLOD terrain;
    terrain.Draw(heightmap, m, v, p);
}

#undef GLSLify

