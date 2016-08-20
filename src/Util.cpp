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
const char* util::Tex2DViz::fs =
    "#version 330\n"
    "in vec2 uv;\n"
    "uniform sampler2D tex;\n"
    "out vec4 fcolor;\n"
    "void main()\n"
    "{\n"
    "    fcolor = texture(tex,uv);\n"
    "}\n";
util::Tex2DViz::Tex2DViz(Texture2D* tex)
{
    mat.pgm = util::setupQuadProgram(fs);
    tex_uniform.loc = glGetUniformLocation(mat.pgm, "tex");
    tex_uniform.tex = tex;
}
void util::Tex2DViz::draw(int x, int y, int w, int h)
{
    tex_uniform.bind();
    util::drawQuad(mat.pgm, x, y, w, h);
}

const char* util::CubemapViz::fs =
    "#version 330\n"
    "in vec2 uv; \n"
    "uniform samplerCube uTex;\n"
    "out vec4 fragColor;\n"
    "\n"
    "vec3 vecFromLatLong(vec2 _uv)\n"
    "{\n"
    "    float pi = 3.14159265;\n"
    "    float twoPi = 2.0*pi;\n"
    "    float phi = _uv.x * twoPi;\n"
    "    float theta = _uv.y * pi;\n"

    "    vec3 result;\n"
    "    result.x = -sin(theta)*sin(phi);\n"
    "    result.y = -cos(theta);\n"
    "    result.z = -sin(theta)*cos(phi);\n"

    "    return result;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 dir = vecFromLatLong(uv);\n"
    "    fragColor = texture(uTex, dir);\n"
    "}\n";
util::CubemapViz::CubemapViz(Cubemap* tex)
{
    mat.pgm = util::setupQuadProgram(fs);
    tex_uniform.loc = glGetUniformLocation(mat.pgm, "tex");
    tex_uniform.tex = tex;
}
void util::CubemapViz::draw(int x, int y, int w, int h)
{
    tex_uniform.bind();
    util::drawQuad(mat.pgm, x, y, w, h);
}
