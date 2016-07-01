#include "Scene.h"

using namespace aa;

void scene::MakeMesh(Mesh& _m, const float* _p, const float* _n, int _vCnt, const unsigned* _i, int _iCnt)
{
    // vao
    glGenVertexArrays(1, &_m.vao);
    glBindVertexArray(_m.vao);

    // fill vertices
    _m.vertices = new Vertex[_vCnt];
    for (int i = 0; i < _vCnt/3; i++)
    {
        _m.vertices[i].p = glm::vec3(_p[3 * i], _p[3 * i+1], _p[3 * i+2]);
        _m.vertices[i].n = glm::vec3(_n[3 * i], _n[3 * i + 1], _n[3 * i + 2]);
        sh::zero(_m.vertices[i].shc);
    }
    _m.verticesCount = _vCnt;

    glGenBuffers(1, &_m.vbo_p);
    glBindBuffer(GL_ARRAY_BUFFER, _m.vbo_p);
    glBufferData(GL_ARRAY_BUFFER, _vCnt * 3 * sizeof(float), _p, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &_m.vbo_n);
    glBindBuffer(GL_ARRAY_BUFFER, _m.vbo_n);
    glBufferData(GL_ARRAY_BUFFER, _vCnt * 3 * sizeof(float), _n, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /*glGenBuffers(1, &_m.vbo_sh);
    glBindBuffer(GL_ARRAY_BUFFER, _m.vbo_sh);
    glBufferData(GL_ARRAY_BUFFER, _vCnt * sizeof(sh::SH_t), 0, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, sizeof(sh::SH_t)/sizeof(float), GL_FLOAT, GL_FALSE, 0, 0);*/

    if (!_i)
    {
        glBindVertexArray(0);
        _m.faces = 0;
        _m.facesCount = 0;
        _m.ibo = 0;
        return;
    }

    // fill indices
    _m.faces = new Face[_iCnt / 3];
    for (int i = 0; i < _iCnt / 3; i++)
        _m.faces[i] = { _i[3 * i], _i[3 * i + 1], _i[3 * i + 2] };
    _m.facesCount = _iCnt / 3;

    glGenBuffers(1, &_m.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _m.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _iCnt * sizeof(unsigned), _i, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void scene::DestroyMesh(Mesh& _m)
{
    glBindVertexArray(0);
    glDeleteBuffers(1, &_m.vbo_p);
    glDeleteBuffers(1, &_m.vbo_n);
    //glDeleteBuffers(1, &_m.vbo_sh);
    glDeleteBuffers(1, &_m.ibo);
    glDeleteVertexArrays(1, &_m.vao);
    _m.vao = 0;
    _m.vbo_p = 0;
    _m.vbo_n = 0;
    _m.vbo_sh = 0;
    _m.ibo = 0;
    delete[] _m.faces;
    delete[] _m.vertices;
    _m.vertices = 0;
    _m.faces = 0;
    _m.facesCount = 0;
    _m.verticesCount = 0;
}

glm::vec3 barycentricCoordinate(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    glm::vec3 bc;
    bc.x = (d11 * d20 - d01 * d21) / denom;
    bc.y = (d00 * d21 - d01 * d20) / denom;
    bc.z = 1.0f - bc.x - bc.y;
    return bc;
}

bool intersectTriangle(scene::Ray& r, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& t)
{
    static const float epsi = 0.01f;

    // compute plane's normal
    glm::vec3 v0v1 = v1 - v0;
    glm::vec3 v0v2 = v2 - v0;
    // no need to normalize
    glm::vec3 N = glm::cross(v0v1,v0v2); // N
    float area2 = N.length();

    // Step 1: finding P

    // check if ray and plane are parallel ?
    float NdotRayDirection = glm::dot(N,r.d);
    if (fabs(NdotRayDirection) < epsi) // almost 0
        return false; // they are parallel so they don't intersect !

    // compute d parameter using equation 2
    float d = glm::dot(N,v0);

    // compute t (equation 3)
    t = (glm::dot(N,r.o) + d) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if (t < 0) return false; // the triangle is behind

    // compute the intersection point using equation 1
    glm::vec3 P = r.o + t * r.d;

    // Step 2: inside-outside test
    glm::vec3 C; // vector perpendicular to triangle's plane

    // edge 0
    glm::vec3 edge0 = v1 - v0;
    glm::vec3 vp0 = P - v0;
    C = glm::cross(edge0,vp0);
    if (glm::dot(N,C) < 0) return false; // P is on the right side

    // edge 1
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 vp1 = P - v1;
    C = glm::cross(edge1, vp1);
    if (glm::dot(N, C) < 0) return false; // P is on the right side

    // edge 2
    glm::vec3 edge2 = v0 - v2;
    glm::vec3 vp2 = P - v2;
    C = glm::cross(edge2, vp2);
    if (glm::dot(N, C) < 0) return false; // P is on the right side;

    return true; // this ray hits the triangle 
}

bool scene::Intersect(Scene& _s, Ray& _r, IntersectionPoint& _ip)
{
    bool hasIntersection = false;
    float tMin = 0.0f;
    IntersectionPoint ip;
    _r.d = glm::normalize(_r.d);

    for (int mi = 0; mi < _s.size(); mi++)
    {
        Mesh& m = _s[mi];
        Ray r;
        r.d = _r.d;
        r.o = _r.o + glm::vec3(m.m[3][0], m.m[3][1], m.m[3][2]);
        for (int i = 0; i < m.facesCount; i++)
        {
            glm::vec3 a = m.vertices[m.faces[i].a].p;
            glm::vec3 b = m.vertices[m.faces[i].b].p;
            glm::vec3 c = m.vertices[m.faces[i].c].p;

            float t;
            bool intersected = intersectTriangle(r, a, b, c, t);

            if (intersected)
            {
                if ((!hasIntersection) || (hasIntersection && t<tMin))
                {
                    tMin = t;
                    ip.meshIndex = mi;
                    ip.faceIndex = i;
                }
                hasIntersection = true;
            }
        }
    }

    if (hasIntersection)
    {
        ip.p = _r.o + tMin * _r.d;
        Mesh& m = _s[ip.meshIndex];
        glm::vec3 a = m.vertices[m.faces[ip.faceIndex].a].p;
        glm::vec3 b = m.vertices[m.faces[ip.faceIndex].b].p;
        glm::vec3 c = m.vertices[m.faces[ip.faceIndex].c].p;
        ip.bc = barycentricCoordinate(ip.p, a, b, c);
    }

    return hasIntersection;
}

struct IntersectDebugShader
{
    GLuint program;
    struct
    {
        GLint m, v, p, ip;
    } u;
    IntersectDebugShader()
    {
        program = glCreateProgram();
        {
            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            static const char* vs_shdr = GLSLify(330,
                in vec3 iPos;
                in vec3 iNormal;
                uniform mat4 uModelMat;
                uniform mat4 uViewMat;
                uniform mat4 uProjMat;
                uniform vec3 uIP;
                out vec3 ip;
                out vec3 normal;
                out vec3 pos;

                void main()
                {
                    gl_Position = uProjMat * uViewMat * uModelMat *vec4(iPos, 1.0);
                    ip = (uProjMat * uViewMat * uModelMat *vec4(uIP, 1.0)).xyz;

                    mat4 no_tr = uModelMat;
                    no_tr[3][0] = 0.0;
                    no_tr[3][1] = 0.0;
                    no_tr[3][2] = 0.0;
                    //normal = (uViewMat * no_tr * vec4(iNormal, 1.0)).xyz;
                    normal = iNormal;
                    pos = gl_Position.xyz;
                }
            );
            glShaderSource(vs, 1, &vs_shdr, NULL);
            glCompileShader(vs);
            glAttachShader(program, vs);

            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fs_shdr = GLSLify(330,
                in vec3 normal;
                in vec3 pos;
                in vec3 ip;
                out vec4 fragColor;
                void main()
                {
                    float d = length(pos - ip);
                    fragColor = vec4(normal*0.5 + vec3(0.5, 0.5, 0.5), 1.0);
                    if (d < 0.03)
                        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
                    //fragColor = vec4(col, 1.0);
                }
            );
            glShaderSource(fs, 1, &fs_shdr, NULL);
            glCompileShader(fs);
            glAttachShader(program, fs);

            glLinkProgram(program);
            glDeleteShader(vs);
            glDeleteShader(fs);
            {
                printf("IntersectDebug shader info:\n");
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
        u.m = glGetUniformLocation(program, "uModelMat");
        u.v = glGetUniformLocation(program, "uViewMat");
        u.p = glGetUniformLocation(program, "uProjMat");
        u.ip = glGetUniformLocation(program, "uIP");
    }

    void SetM(glm::mat4 m)
    {
        glUniformMatrix4fv(u.m, 1, GL_FALSE, glm::value_ptr(m));
    }

    void SetV(glm::mat4 v)
    {
        glUniformMatrix4fv(u.v, 1, GL_FALSE, glm::value_ptr(v));
    }

    void SetP(glm::mat4 p)
    {
        glUniformMatrix4fv(u.p, 1, GL_FALSE, glm::value_ptr(p));
    }

    void SetIP(glm::vec3 ip)
    {
        glUniform3f(u.ip, ip.x, ip.y, ip.z);
    }

    void Use()
    {
        glUseProgram(program);
    }

    ~IntersectDebugShader()
    {
        glDeleteProgram(program);
        program = 0;
    }
};

void scene::DrawIntersectDebug(Scene& _s, glm::mat4 v, glm::mat4 p, glm::vec3 ip)
{
    static IntersectDebugShader ids;
    ids.Use();
    ids.SetV(v);
    ids.SetP(p);
    ids.SetIP(ip);

    for (int i = 0; i < _s.size(); i++)
    {
        Mesh m = _s[i];
        ids.SetM(m.m);
        glBindVertexArray(m.vao);
        glDrawElements(GL_TRIANGLES, m.facesCount * 3, GL_UNSIGNED_INT, 0);
    }
}

