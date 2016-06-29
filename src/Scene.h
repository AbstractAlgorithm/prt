#pragma once

#include "Render.h"
#include "SH.h"
#include <vector>

namespace aa
{
    namespace scene
    {
        struct Vertex
        {
            glm::vec3 p, n;
            sh::SH_t shc;
        };

        struct Face
        {
            int a, b, c;
        };

        struct Mesh
        {
            GLuint vao, vbo_p, vbo_n, vbo_sh, ibo;

            Vertex* vertices;
            Face* faces;

            int verticesCount;
            int facesCount;
        };

        typedef std::vector<Mesh> Scene;

        void MakeMesh(Mesh& _m, const float* _p, const float* _n, int _vCnt, const int* _i = 0, int _iCnt = 0);
        void DestroyMesh(Mesh& _m);

        //void Intersect();


    }
}





