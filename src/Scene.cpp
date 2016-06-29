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
