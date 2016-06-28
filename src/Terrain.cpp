#include "Terrain.h"

struct TerrainLOD
{
    GLuint program, vao, vbo;
    GLint uloc_heightmap, uloc_m, uloc_v, uloc_p, uloc_ss;
    static const char patchCount = 64;

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
                gl_Position = vec4(position.x, height / 3.0, position.y, 1.0);
            }
            );
            glShaderSource(vs, 1, &vs_shdr, NULL);
            glCompileShader(vs);
            glAttachShader(program, vs);

            // tessellation control shader
            GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
            const char* tcs_shdr = GLSLify(400,
                layout(vertices = 4) out;

            uniform mat4 uModelMat;
            uniform mat4 uViewMat;
            uniform mat4 uProjMat;
            uniform vec2 uScreenSize;
            const float lodFactor = 10.0f;

            vec4 project(vec4 vertex){
                vec4 result = uProjMat * uViewMat * uModelMat * vertex;
                result /= result.w;
                return result;
            }

            float level(vec2 v0, vec2 v1)
            {
                return clamp(distance(v0, v1) / lodFactor, 1.0, 64.0);
            }

            vec2 screen_space(vec4 vertex){
                return vertex.xy * (uScreenSize*0.5);
            }

            void main()
            {
                if (gl_InvocationID == 0)
                {
                    vec4 v0 = project(gl_in[0].gl_Position);
                    vec4 v1 = project(gl_in[1].gl_Position);
                    vec4 v2 = project(gl_in[2].gl_Position);
                    vec4 v3 = project(gl_in[3].gl_Position);

                    vec2 ss0 = screen_space(v0);
                    vec2 ss1 = screen_space(v1);
                    vec2 ss2 = screen_space(v2);
                    vec2 ss3 = screen_space(v3);

                    float e0 = level(ss1, ss2);
                    float e1 = level(ss0, ss1);
                    float e2 = level(ss3, ss0);
                    float e3 = level(ss2, ss3);

                    gl_TessLevelInner[0] = mix(e1, e2, 0.5);
                    gl_TessLevelInner[1] = mix(e0, e3, 0.5);
                    gl_TessLevelOuter[0] = e0;
                    gl_TessLevelOuter[1] = e1;
                    gl_TessLevelOuter[2] = e2;
                    gl_TessLevelOuter[3] = e3;

                    /*gl_TessLevelInner[0] = 1.0f;
                    gl_TessLevelInner[1] = 1.0f;

                    gl_TessLevelOuter[0] = 1.0f;
                    gl_TessLevelOuter[1] = 1.0f;
                    gl_TessLevelOuter[2] = 1.0f;
                    gl_TessLevelOuter[3] = 1.0f;*/
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
                layout(quads, equal_spacing, ccw) in;
            out vec2 uv;
            out float h;

            uniform sampler2D uHeightmap;
            uniform mat4 uModelMat;
            uniform mat4 uViewMat;
            uniform mat4 uProjMat;

            //float rand(float n){return fract(sin(n) * 43758.5453123);}

            void main()
            {
                float u = gl_TessCoord.x;
                float v = gl_TessCoord.y;

                vec4 a = mix(gl_in[1].gl_Position, gl_in[0].gl_Position, u);
                vec4 b = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, u);
                vec4 pos = mix(a, b, v);
                uv = pos.xz;
                float height = texture(uHeightmap, uv).r;
                h = height;
                gl_Position = uProjMat * uViewMat * uModelMat * vec4(uv.x, height / 3.0, uv.y, 1.0);
            }
            );
            glShaderSource(tes, 1, &tes_shdr, NULL);
            glCompileShader(tes);
            glAttachShader(program, tes);

            // fragment shader
            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fs_shdr = GLSLify(400,
                in vec2 uv;
            in float h;
            uniform sampler2D uHeightmap;
            out vec4 fragColor;

            void main()
            {
                //float h = texture(uHeightmap, uv).r;
                fragColor = vec4(uv, h, 1.0);
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
                printf("Terrain tessellation shader info:\n");
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
                printf("%s\n", infoLog);
                delete[] infoLog;
            }
        }
        uloc_heightmap = glGetUniformLocation(program, "uHeightmap");
        uloc_ss = glGetUniformLocation(program, "uScreenSize");
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

        glDisable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);
        RECT hrect;
        GetClientRect(aa::window::g_hWnd, &hrect);
        glUniform2f(uloc_ss, (float)hrect.right, (float)hrect.bottom);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightmap);
        glUniform1i(uloc_heightmap, 0);
        glUniformMatrix4fv(uloc_m, 1, GL_FALSE, glm::value_ptr(m));
        glUniformMatrix4fv(uloc_v, 1, GL_FALSE, glm::value_ptr(v));
        glUniformMatrix4fv(uloc_p, 1, GL_FALSE, glm::value_ptr(p));

        glBindVertexArray(vao);
        glDrawArrays(GL_PATCHES, 0, patchCount*patchCount * 4);
        //glDisable(GL_CULL_FACE);
    }
};

void aa::terrain::DrawTess(GLuint heightmap, glm::mat4 m, glm::mat4 v, glm::mat4 p)
{
    static TerrainLOD terrain;
    terrain.Draw(heightmap, m, v, p);
}

