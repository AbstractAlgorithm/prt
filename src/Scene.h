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

        struct Mesh
        {
            GLuint vao, vbo_p, vbo_n, vbo_sh;

            Vertex* vertices;
            int indicesCount;
        };

        typedef std::vector<Mesh> Scene;

        void MakeMesh(Mesh& _m, float* _p, float* _n, int verticesCount);
        void DestroyMesh(Mesh& _m);

        void Intersect();


    }
}





