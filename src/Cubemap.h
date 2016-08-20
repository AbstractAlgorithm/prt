#pragma once

#include "WinEntry.h"

namespace aa
{
    namespace gfx
    {
        struct Cubemap
        {
            GLuint tex;
            Cubemap(unsigned dim = 0);
            bool bmp(const char* filename[]);
            void bind();
            ~Cubemap();
        };
    }
}
