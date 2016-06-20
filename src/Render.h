#include "WinEntry.h"

namespace aa
{
    namespace render
    {
        void DrawTexturedQuad(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height);
        GLuint CreateTexture2D(char const* filename);
        GLuint CreteTextureCubemap(const char* filenames[6]);
        void DrawCubemapAsLatlong(GLuint cubemap, unsigned x, unsigned y, unsigned width, unsigned height);
        
        void DrawLODTerrain(GLuint heightmap, glm::mat4 m, glm::mat4 v, glm::mat4 p, bool wireframe = false);
    }
}
