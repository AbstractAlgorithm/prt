#include "Render.h"

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

GLuint aa::render::CreateTexture2D(char const* Filename)
{
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (fillBMP(Filename) != S_OK)
        return 0;
    return tex;
}

#define GLSLify(version, shader) "#version " #version "\n" #shader

struct QuadRenderer
{
    GLuint quad, program;
    GLint uloc_tex, uloc_res, uloc_dim;

    QuadRenderer()
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
        GLfloat verts[12] = { -1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1 };
        glGenBuffers(1, &quad);
        glBindBuffer(GL_ARRAY_BUFFER, quad);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        // bind and setup
        RECT hrect;
        GetClientRect(aa::window::g_hWnd, &hrect);
        glUseProgram(program);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glActiveTexture(GL_TEXTURE0);
        glUniform2f(uloc_res, (float)hrect.right, (float)hrect.bottom);
        glUniform1i(uloc_tex, 0);
    }

    void draw(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform4f(uloc_dim, (float)x, (float)y, (float)width, (float)height);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    ~QuadRenderer()
    {
        glDeleteBuffers(1, &quad);
        quad = 0;

        glDeleteProgram(program);
        program = 0;
    }
};

#undef GLSLify

void aa::render::DrawTexturedQuad(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height)
{
    static QuadRenderer qr;
    qr.draw(texture, x, y, width, height);
}