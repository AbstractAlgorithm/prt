#include "WinEntry.h"

namespace aa
{
    namespace render
    {
        GLuint CreateTexture2D(char const* filename);
        void DrawTexturedQuad(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height);

        GLuint CreateCubemapEmpty(glm::ivec2 res);
        GLuint CreteTextureCubemap(const char* filenames[6]);
        void DrawCubemapAsLatlong(GLuint cubemap, unsigned x, unsigned y, unsigned width, unsigned height);
        void FillCubemap(GLuint cubemap, glm::ivec2 res, glm::vec3 position, void(*drawWorldFunc)(glm::mat4 v, glm::mat4 p));
        void RenderSkybox(GLuint cubemap, glm::mat4 v, glm::mat4 p);
        
        void DrawLODTerrain(GLuint heightmap, glm::mat4 m, glm::mat4 v, glm::mat4 p);
    }
}
