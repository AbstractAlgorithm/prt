#include "WinEntry.h"

namespace aa
{
    namespace render
    {
        GLuint CreateTexture2D(char const* filename);
        void DrawTexturedQuad(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height);
        void DrawLODTerrain(GLuint heightmap, glm::mat4 m, glm::mat4 v, glm::mat4 p);
    }
}
