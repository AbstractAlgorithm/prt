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
            unsigned a, b, c;
        };

        struct Ray
        {
            glm::vec3 o, d;
        };

        struct Mesh
        {
            GLuint vao, vbo_p, vbo_n, vbo_sh, ibo;

            Vertex* vertices;
            Face* faces;

            int verticesCount;
            int facesCount;

            glm::mat4 m;
        };

        struct IntersectionPoint
        {
            int meshIndex;
            int faceIndex;
            glm::vec3 p;
            glm::vec3 bc;
        };

        typedef std::vector<Mesh> Scene;

        void MakeMesh(Mesh& _m, const float* _p, const float* _n, int _vCnt, const unsigned* _i = 0, int _iCnt = 0);
        void DestroyMesh(Mesh& _m);

        bool Intersect(Scene& _s, Ray& _r, IntersectionPoint& _ip);

        void DrawIntersectDebug(Scene& _s, glm::mat4 v, glm::mat4 p, glm::vec3 ip);
    }
}





