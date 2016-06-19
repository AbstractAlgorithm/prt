#include "WinEntry.h"

namespace aa
{
    namespace render
    {
        GLuint CreateTexture2D(char const* Filename);
        void DrawTexturedQuad(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height);
    }
}
