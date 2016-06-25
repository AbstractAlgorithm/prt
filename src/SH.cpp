#include "SH.h"
#include "Render.h"

// -----------------------------------------------=-=- Spherical Harmonics -=-=-

void aa::sh::make(SH_t& sh, const double* values, int coeffsCnt)
{
    assert(coeffsCnt < SH_CHANNELS*SH_BANDS*SH_BANDS);
    for (unsigned i = 0; i < coeffsCnt; i++)
        sh[i % 3][i / 3] = values[i];
}

void aa::sh::zero(SH_t& sh)
{
    for (unsigned ci = 0; ci < SH_CHANNELS; ci++)
    for (unsigned ii = 0; ii < SH_BANDS*SH_BANDS; ii++)
        sh[ci][ii] = 0.0;
}

void aa::sh::copy(SH_t& a, const SH_t& b)
{
    for (unsigned ci = 0; ci < SH_CHANNELS; ci++)
    for (unsigned ii = 0; ii < SH_BANDS*SH_BANDS; ii++)
        a[ci][ii] = b[ci][ii];
}

void aa::sh::add(SH_t& a, const SH_t& b)
{
    for (unsigned ci = 0; ci < SH_CHANNELS; ci++)
    for (unsigned ii = 0; ii < SH_BANDS*SH_BANDS; ii++)
        a[ci][ii] += b[ci][ii];
}

void aa::sh::add(SH_t& c, const SH_t& a, const SH_t& b)
{
    for (unsigned ci = 0; ci < SH_CHANNELS; ci++)
    for (unsigned ii = 0; ii < SH_BANDS*SH_BANDS; ii++)
        c[ci][ii] = a[ci][ii] + b[ci][ii];
}

void aa::sh::sub(SH_t& a, const SH_t& b)
{
    for (unsigned ci = 0; ci < SH_CHANNELS; ci++)
    for (unsigned ii = 0; ii < SH_BANDS*SH_BANDS; ii++)
        a[ci][ii] -= b[ci][ii];
}

void aa::sh::sub(SH_t& c, const SH_t& a, const SH_t& b)
{
    for (unsigned ci = 0; ci < SH_CHANNELS; ci++)
    for (unsigned ii = 0; ii < SH_BANDS*SH_BANDS; ii++)
        c[ci][ii] = a[ci][ii] - b[ci][ii];
}

void aa::sh::mul(SH_t& a, double s)
{
    for (unsigned ci = 0; ci < SH_CHANNELS; ci++)
    for (unsigned ii = 0; ii < SH_BANDS*SH_BANDS; ii++)
        a[ci][ii] *= s;
}

void aa::sh::div(SH_t& a, double s)
{
    for (unsigned ci = 0; ci < SH_CHANNELS; ci++)
    for (unsigned ii = 0; ii < SH_BANDS*SH_BANDS; ii++)
        a[ci][ii] /= s;
}

// sin(theta) * cos(phi)    => x
// sin(theta) * sin(phi)    => y
// cos(theta)               => z

// -----------------------------------------------------------------------------

// P{m,l}
// l - band index  in [0,oo]
// m - takes value in [0, l]

// 1. (l-m)P{m,l} = x * (2l-1) * P{m,l-1} - (l+m-1) * P{m,l-2}
// 2. P{m,m} = (-1)^m * (2m-1)!! * (1-x*x)^(m/2)
// 3. P{m,m+1} = x(2m+1) * P{m,m}

double P(int l, int m, double x)
{
    double pmm = 1.0;
    if (m > 0)
    {
        double somx2 = sqrt((1.0 - x)*(1.0 + x));
        double fact = 1.0;
        for (int i = 1; i <= m; ++i)
        {
            pmm *= (-fact) * somx2;
            fact += 2.0;
        }
    }
    if (l == m) return pmm;
    double pmmp1 = x*(2.0*m + 1.0)*pmm;
    if (l == m + 1) return pmmp1;
    double pll = 0.0;
    for (int ll = m + 2; ll <= l; ++ll)
    {
        pll = ((2.0*ll + 1.0)*x*pmmp1 - (ll + m - 1.0)*pmm) / (ll - m);
        pmm = pmmp1;
        pmmp1 = pll;
    }
    return pll;
}

// -----------------------------------------------------------------------------

long long factorial(int n)
{
    const long long table[15] =
    {
        1,
        1,
        2,
        6,
        24,
        120,
        720,
        5040,
        40320,
        362880,
        3628800,
        39916800,
        479001600,
        6227020800,
        87178291200
    };
    assert(n < 16);
    return table[n];
}


double K(int l, int m)
{
    double temp = ((2.0*l + 1.0) * factorial(l - m)) / (PI4*factorial(l + m));
    return sqrt(temp);
}

// -----------------------------------------------------------------------------

// Y{m,l}
// l - band index  in [0,oo]
// m - takes value in [-l,l]

// Y{m,l,theta,phi}
// m = 0: K{0,l} * P{0,l,cos(theta)}
// m < 0: sqrt(2) * K{m,l} * sin(-m*phi) * P{-m,l,cos(theta)}
// m > 0: sqrt(2) * K{m,l} * cos( m*phi) * P{ m,l,cos(theta)}
// where K{m,l} = sqrt( (2*l+1)/(4*pi) * (l-|m|)!/(l+m)! )

// Y{m,l} = Y{i} where i = l(l+1) + m

/*
Returns a point sample of a SH basis function
l:      [0..N] band
m:      [-l..l]
theta:  [0..Pi]
phi:    [0..2*Pi]
*/
double Y(int l, int m, double theta, double phi)
{
    if (m == 0)     return K(l, 0) *P(l, m, cos(theta));
    else if (m < 0) return SQRT_2 * K(l, m) * sin(-m*phi) * P(l, -m, cos(theta));
    else            return SQRT_2 * K(l, m) * cos(m*phi) * P(l, m, cos(theta));
}

// c{i} = int{S} [ f(s) * Y{i,s} ] 
// _f(s) = sum{i=0,n*n} [c{i} * Y{i}]

// ----------------------------------------------------=-=- cool functions -=-=-

struct SHGenerator
{
    GLuint program;

    SHGenerator()
    {
        GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
        static const char* shader_src = GLSLify(430,
            layout(local_size_x = 16, local_size_y = 16) in;

            void main()
            {

            }
        );
        glShaderSource(cs, 1, &shader_src, NULL);
        glCompileShader(cs);

        program = glCreateProgram();
        glAttachShader(program, cs);
        glLinkProgram(program);
        glDeleteShader(cs);
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

    ~SHGenerator()
    {
        glDeleteProgram(program);
        program = 0;
    }

    void generate(GLuint cubemap, unsigned size, aa::sh::SH_t& sh)
    {
        // setup uniforms
        // bind cubemap
        // declare workgroup size depenging on the size
        // allocate memory for the results
        // bind the memory for the result to the shader
        // invoke shader (dispatch)

        /*for (unsigned fi = 0; fi < 6; fi++)
        {
        GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + fi;
        float* positions = (vmath::vec4 *)
        glMapBufferRange(GL_ARRAY_BUFFER,
        0,
        PARTICLE_COUNT * sizeof(vmath::vec4),
        GL_MAP_WRITE_BIT |
        GL_MAP_INVALIDATE_BUFFER_BIT);
        }*/

    }
};

void aa::sh::GenerateCoefficients(GLuint cubemap, unsigned size, SH_t& sh)
{
    static SHGenerator shg;
    shg.generate(cubemap, size, sh);
}

// -----------------------------------------------------------------------------

struct SHPainter
{
    GLuint vao, vbo, program;
    GLint uloc_shc, uloc_res, uloc_dim;

    SHPainter()
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
                uniform vec3 uSH[9];
                out vec4 fragColor;

                vec3 vecFromLatLong(vec2 _uv)
                {
                    float pi = 3.14159265;
                    float twoPi = 2.0*pi;
                    float phi = _uv.x * twoPi;
                    float theta = _uv.y * pi;

                    vec3 result;
                    result.x = -sin(theta)*sin(phi);
                    result.y = -cos(theta);
                    result.z = -sin(theta)*cos(phi);

                    return result;
                }

                const float k01 = 0.2820947918; // sqrt( 1/PI)/2
                const float k02 = 0.4886025119; // sqrt( 3/PI)/2
                const float k03 = 1.0925484306; // sqrt(15/PI)/2
                const float k04 = 0.3153915652; // sqrt( 5/PI)/4
                const float k05 = 0.5462742153; // sqrt(15/PI)/4

                vec3 evalSH(vec3 _dir)
                {
                    vec3 nn = normalize(_dir);

                    float sh[9];
                    sh[0] = k01;
                    sh[1] = -k02*nn.y;
                    sh[2] = k02*nn.z;
                    sh[3] = -k02*nn.x;
                    sh[4] = k03*nn.y*nn.x;
                    sh[5] = -k03*nn.y*nn.z;
                    sh[6] = k04*(3.0*nn.z*nn.z - 1.0);
                    sh[7] = -k03*nn.x*nn.z;
                    sh[8] = k05*(nn.x*nn.x - nn.y*nn.y);

                    vec3 rgb = vec3(0.0);
                    rgb += uSH[0] * sh[0];
                    rgb += uSH[1] * sh[1];
                    rgb += uSH[2] * sh[2];
                    rgb += uSH[3] * sh[3];
                    rgb += uSH[4] * sh[4];
                    rgb += uSH[5] * sh[5];
                    rgb += uSH[6] * sh[6];
                    rgb += uSH[7] * sh[7];
                    rgb += uSH[8] * sh[8];
                    return rgb;
                }

                void main()
                {
                    vec3 dir = vecFromLatLong(uv);
                    vec3 col = evalSH(dir);
                    fragColor = vec4(col, 1.0);
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
        uloc_shc = glGetUniformLocation(program, "uSH");
        uloc_dim = glGetUniformLocation(program, "uDim");

        // init quad
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

    ~SHPainter()
    {
        glBindVertexArray(0);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        vbo = 0;
        vao = 0;

        glDeleteProgram(program);
        program = 0;
    }

    void draw(aa::sh::SH_t sh, glm::ivec2 pos, glm::uvec2 dim)
    {
        glUseProgram(program);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        int dims[4];
        glGetIntegerv(GL_VIEWPORT, dims);
        glUniform2f(uloc_res, dims[2], dims[3]);
        glUniform4f(uloc_dim, (float)pos.x, (float)pos.y, (float)dim.x, (float)dim.y);
        GLfloat shc_[27];
        for (int i = 0; i < 27; i++)
        {
            shc_[i] = (float)(sh[i%3][i/3]);
        }
        glUniform3fv(uloc_shc, 9, shc_);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

void aa::sh::DrawLatlong(SH_t sh, glm::ivec2 pos, glm::uvec2 dim)
{
    static SHPainter shp;
    shp.draw(sh, pos, dim);
}

