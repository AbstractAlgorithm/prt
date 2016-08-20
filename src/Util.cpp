#include "Util.h"

using namespace aa;

GLuint util::setupQuadProgram(const char* fragShdrSrc)
{
    const char* fsquadvs =
        "#version 330\n"
        "out vec2 uv;\n"
        "void main()\n"
        "{\n"
        "    vec2 pos;\n"
        "    if (0 == gl_VertexID) pos = vec2(+1.0, +1.0);\n"
        "    if (1 == gl_VertexID) pos = vec2(-1.0, +1.0);\n"
        "    if (2 == gl_VertexID) pos = vec2(-1.0, -1.0);\n"
        "    if (3 == gl_VertexID) pos = vec2(+1.0, -1.0);\n"
        "    gl_Position = vec4(pos, 0.0, 1.0);\n"
        "    uv = pos * vec2(0.5, -0.5) + 0.5;\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &fsquadvs, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragShdrSrc, NULL);
    glCompileShader(fs);

    GLuint pgm = glCreateProgram();
    glAttachShader(pgm, vs);
    glAttachShader(pgm, fs);
    glLinkProgram(pgm);

    return pgm;
}
void util::drawQuad(GLuint pgm, int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
    glDisable(GL_CULL_FACE);
    glUseProgram(pgm);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
