#pragma once

#include "WinEntry.h"
#include "Textures.h"
#include "Material.h"
#include <cmath>
#include <cstdint>
#include <cassert>
#include <vector>

using namespace aa::gfx;

namespace aa
{
    namespace util
    {
        GLuint setupQuadProgram(const char* fragShdrSrc);
        void drawQuad(int x, int y, int w, int h);

        struct Tex2DViz
        {
            Material mat;
            tex2d_u tex_uniform;
            Tex2DViz(Texture2D* tex = 0);
            void draw(int x, int y, int w, int h);
        private:
            static const char* fs;
        };
        struct CubemapViz
        {
            Material matLatlong;
            Material matProbe;
            texcm_u texLatlong;
            texcm_u texProbe;
            CubemapViz(Cubemap* tex = 0);
            void drawLatlong(int x, int y, int w, int h);
            void drawProbe(int x, int y, unsigned dim);
        private:
            static const char* latlong_fs;
            static const char* probe_fs;
        };
    }
}
