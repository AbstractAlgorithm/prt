#include "Scene.h"

using namespace aa;

void scene::MakeMesh(Mesh& _m, const float* _p, const float* _n, int _vCnt, const int* _i, int _iCnt)
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

    glGenBuffers(1, &_m.vbo_sh);
    glBindBuffer(GL_ARRAY_BUFFER, _m.vbo_sh);
    glBufferData(GL_ARRAY_BUFFER, _vCnt * sizeof(sh::SH_t), 0, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, sizeof(sh::SH_t)/sizeof(float), GL_FLOAT, GL_FALSE, 0, 0);

    if (!_i)
    {
        glBindVertexArray(0);
        return;
    }

    // fill indices
    _m.faces = new Face[_iCnt / 3];
    for (int i = 0; i < _iCnt / 3; i++)
        _m.faces[i] = { _i[3 * i], _i[3 * i + 1], _i[3 * i + 2] };
    _m.facesCount = _iCnt / 3;

    glGenBuffers(1, &_m.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _m.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _iCnt * sizeof(int), _i, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void scene::DestroyMesh(Mesh& _m)
{
    glBindVertexArray(0);
    glDeleteBuffers(1, &_m.vbo_p);
    glDeleteBuffers(1, &_m.vbo_n);
    glDeleteBuffers(1, &_m.vbo_sh);
    glDeleteBuffers(1, &_m.ibo);
    glDeleteVertexArrays(1, &_m.vao);
    _m.vao = 0;
    _m.vbo_p = 0;
    _m.vbo_n = 0;
    _m.vbo_sh = 0;
    _m.ibo = 0;
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
    IntersectionPoint ip;

    for (int mi = 0; mi < _s.size(); mi++)
    {
        Mesh& m = _s[mi];
        for (int i = 0; i < m.facesCount; i++)
        {
            glm::vec3 a = m.vertices[m.faces[i].a].p;
            glm::vec3 b = m.vertices[m.faces[i].b].p;
            glm::vec3 c = m.vertices[m.faces[i].c].p;

            float t;
            bool intersected = intersectTriangle(_r, a, b, c, t);

            if (intersected)
            {
                if ((!hasIntersection) || (hasIntersection && t<ip.t))
                {
                    ip.t = t;
                    ip.meshIndex = mi;
                    ip.faceIndex = i;
                }
                hasIntersection = true;
            }
        }
    }

    if (hasIntersection)
    {
        glm::vec3 p = _r.o + ip.t * _r.d;
        Mesh& m = _s[ip.meshIndex];
        glm::vec3 a = m.vertices[m.faces[ip.faceIndex].a].p;
        glm::vec3 b = m.vertices[m.faces[ip.faceIndex].b].p;
        glm::vec3 c = m.vertices[m.faces[ip.faceIndex].c].p;
        ip.bc = barycentricCoordinate(p, a, b, c);
    }

    return hasIntersection;
}
