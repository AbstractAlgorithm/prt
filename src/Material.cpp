#include "Material.h"

using namespace aa::gfx;

Shader::Shader(GLenum _ty)
    : shdr(glCreateShader(_ty))
    , ty(_ty)
{}
void Shader::src(const char* source)
{
    glShaderSource(shdr, 1, &source, NULL);
    glCompileShader(shdr);
}
void Shader::log()
{
    int infologLen = 0;
    int charsWritten = 0;
    GLchar *infoLog = NULL;
    glGetShaderiv(shdr, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 0)
    {
        infoLog = (GLchar*)malloc(infologLen);
        if (infoLog == NULL)
            return;
        glGetShaderInfoLog(shdr, infologLen, &charsWritten, infoLog);
    }
    printf("%s\n", infoLog);
    delete[] infoLog;
}
Shader::~Shader()
{
    glDeleteShader(shdr);
    shdr = 0;
}

VertexShader::VertexShader() : Shader(GL_VERTEX_SHADER) {}
TessCtrlShader::TessCtrlShader() : Shader(GL_TESS_CONTROL_SHADER) {}
TessEvalShader::TessEvalShader() : Shader(GL_TESS_EVALUATION_SHADER) {}
GeometryShader::GeometryShader() : Shader(GL_GEOMETRY_SHADER) {}
FragmentShader::FragmentShader() : Shader(GL_FRAGMENT_SHADER) {}
ComputeShader::ComputeShader() : Shader(GL_COMPUTE_SHADER) {}

Program::Program()
{}
Program::Program(Shader s1)
{
    pgm = glCreateProgram();
    glAttachShader(pgm, s1.shdr);
    glLinkProgram(pgm);
}
Program::Program(Shader s1, Shader s2)
{
    pgm = glCreateProgram();
    glAttachShader(pgm, s1.shdr);
    glAttachShader(pgm, s2.shdr);
    glLinkProgram(pgm);
}
Program::Program(Shader s1, Shader s2, Shader s3)
{
    pgm = glCreateProgram();
    glAttachShader(pgm, s1.shdr);
    glAttachShader(pgm, s2.shdr);
    glAttachShader(pgm, s3.shdr);
    glLinkProgram(pgm);
}
Program::Program(Shader s1, Shader s2, Shader s3, Shader s4)
{
    pgm = glCreateProgram();
    glAttachShader(pgm, s1.shdr);
    glAttachShader(pgm, s2.shdr);
    glAttachShader(pgm, s3.shdr);
    glAttachShader(pgm, s4.shdr);
    glLinkProgram(pgm);
}
Program::Program(Shader s1, Shader s2, Shader s3, Shader s4, Shader s5)
{
    pgm = glCreateProgram();
    glAttachShader(pgm, s1.shdr);
    glAttachShader(pgm, s2.shdr);
    glAttachShader(pgm, s3.shdr);
    glAttachShader(pgm, s4.shdr);
    glAttachShader(pgm, s5.shdr);
    glLinkProgram(pgm);
}
Program::~Program()
{
    glDeleteProgram(pgm);
    pgm = 0;
}
void Program::log()
{
    int infologLen = 0;
    int charsWritten = 0;
    GLchar *infoLog = NULL;
    glGetProgramiv(pgm, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 0)
    {
        infoLog = (GLchar*)malloc(infologLen);
        if (infoLog == NULL)
            return;
        glGetProgramInfoLog(pgm, infologLen, &charsWritten, infoLog);
    }
    printf("%s\n", infoLog);
    delete[] infoLog;
}
void Program::use()
{
    glUseProgram(pgm);
}

float_u::float_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void float_u::bind() { glUniform1f(loc, val); }
vec2_u::vec2_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void vec2_u::bind() { glUniform2f(loc, val.x, val.y); }
vec3_u::vec3_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void vec3_u::bind() { glUniform3f(loc, val.x, val.y, val.z); }
vec4_u::vec4_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void vec4_u::bind() { glUniform4f(loc, val.x, val.y, val.z, val.w); }
int_u::int_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void int_u::bind() { glUniform1i(loc, val); }
ivec2_u::ivec2_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void ivec2_u::bind() { glUniform2i(loc, val.x, val.y); }
ivec3_u::ivec3_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void ivec3_u::bind() { glUniform3i(loc, val.x, val.y, val.z); }
ivec4_u::ivec4_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void ivec4_u::bind() { glUniform4i(loc, val.x, val.y, val.z, val.w); }
uint_u::uint_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void uint_u::bind() { glUniform1ui(loc, val); }
uvec2_u::uvec2_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void uvec2_u::bind() { glUniform2ui(loc, val.x, val.y); }
uvec3_u::uvec3_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void uvec3_u::bind() { glUniform3ui(loc, val.x, val.y, val.z); }
uvec4_u::uvec4_u(Material& m, const char* name) { loc = glGetUniformLocation(m.pgm, name); }
void uvec4_u::bind() { glUniform4ui(loc, val.x, val.y, val.z, val.w); }
tex2d_u::tex2d_u()
    : slot(0)
    , tex(0)
    , loc(-1)
{}
tex2d_u::tex2d_u(Material& m, const char* name, Texture2D* tx, GLuint slt)
    : slot(slt)
    , tex(tx)
    , loc(glGetUniformLocation(m.pgm, name))
{}
void tex2d_u::bind()
{
    if (!tex)
        return;
    glActiveTexture(GL_TEXTURE0 + slot);
    tex->bind();
    glUniform1i(loc, slot);
}
texcm_u::texcm_u()
    : slot(0)
    , tex(0)
    , loc(-1)
{}
texcm_u::texcm_u(Material& m, const char* name, Cubemap* tx, GLuint slt)
    : slot(slt)
    , tex(tx)
    , loc(glGetUniformLocation(m.pgm, name))
{}
void texcm_u::bind()
{
    if (!tex)
        return;
    glActiveTexture(GL_TEXTURE0 + slot);
    tex->bind();
    glUniform1i(loc, slot);
}
