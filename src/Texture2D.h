#pragma once

#include "WinEntry.h"

namespace aa
{
    namespace gfx
    {
        struct Texture2D
        {
            GLuint tex;
            Texture2D();
            bool bmp(const char* filename);
            void bind();
            ~Texture2D();
        };
    }
}
