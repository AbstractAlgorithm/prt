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

    VertexShader vs;
    vs.src(fsquadvs);

    FragmentShader fs;
    fs.src(fragShdrSrc);

    GLuint pgm = glCreateProgram();
    glAttachShader(pgm, vs.shdr);
    glAttachShader(pgm, fs.shdr);
    glLinkProgram(pgm);

    return pgm;
}
void util::drawQuad(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
    glDisable(GL_CULL_FACE);
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
    mat.use();
    tex_uniform.bind();
    util::drawQuad(x, y, w, h);
}
const char* util::CubemapViz::latlong_fs =
    "#version 330\n"
    "in vec2 uv; \n"
    "uniform samplerCube tex;\n"
    "out vec4 fcolor;\n"
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
    "    fcolor = texture(tex, dir);\n"
    "}\n";
const char* util::CubemapViz::probe_fs =
    "#version 330\n"
    "in vec2 uv;\n"
    "uniform samplerCube tex;\n"
    "out vec4 fcolor;\n"
    "void main()\n"
    "{\n"
    "    vec2 p = uv*2.0 - vec2(1.0); \n"
    "    if (length(p) < 1.0)\n"
    "    {\n"
    "        float theta = acos(p.x);\n"
    "        float phi = acos(p.y);\n"
    "        float sr = sin(phi);\n"
    "        vec3 n;\n"
    "        n.x = p.x;\n"
    "        n.y = p.y;\n"
    "        n.z = sqrt(sr*sr - p.x*p.x);\n"
    "        n = normalize(n);\n"
    "        vec3 camera = vec3(0.0, 0.0, 30.0);\n"
    "        vec3 ray = n - camera;\n"
    "        vec3 rRay = reflect(ray, n);\n"
    "        fcolor = texture(tex, rRay);\n"
    "    }\n"
    "    else\n"
    "        discard;\n"
    "}";
util::CubemapViz::CubemapViz(Cubemap* tex)
{
    matLatlong.pgm = util::setupQuadProgram(latlong_fs);
    texLatlong.loc = glGetUniformLocation(matLatlong.pgm, "tex");
    texLatlong.tex = tex;
    matProbe.pgm = util::setupQuadProgram(probe_fs);
    matProbe.log();
    texProbe.loc = glGetUniformLocation(matProbe.pgm, "tex");
    texProbe.tex = tex;
}
void util::CubemapViz::drawLatlong(int x, int y, int w, int h)
{
    matLatlong.use();
    texLatlong.bind();
    util::drawQuad(x, y, w, h);
}
void util::CubemapViz::drawProbe(int x, int y, unsigned dim)
{
    matProbe.use();
    texProbe.bind();
    util::drawQuad(x, y, dim, dim);
}
