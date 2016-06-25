#pragma once

#include "WinEntry.h"

#define GLSLify(version, shader) "#version " #version "\n" #shader
#define LFX_ERRCHK(glFn) \
do { \
glFn; \
GLenum err = glGetError(); \
const GLubyte* errmsg = gluErrorString(err); \
if (err != GL_NO_ERROR)    \
    printf("ERROR: 0x%x (%s)\n%s : %d\n", err, errmsg, __FILE__, __LINE__); \
} while (0)

namespace aa
{
    namespace render
    {
        GLuint CreateTexture2D(char const* filename);
        void DrawTexturedQuad(GLuint texture, unsigned x, unsigned y, unsigned width, unsigned height);

        GLuint CreateCubemapEmpty(glm::ivec2 res);
        GLuint CreteTextureCubemap(const char* filenames[6]);
        void DrawCubemapAsLatlong(GLuint cubemap, unsigned x, unsigned y, unsigned width, unsigned height);
        void DrawCubemapProbe(GLuint cubemap, unsigned x, unsigned y, unsigned dim);
        void FillCubemap(GLuint cubemap, unsigned res, glm::vec3 position, void(*drawWorldFunc)(glm::mat4 v, glm::mat4 p), void(*callback)(unsigned, unsigned) = 0);
        void RenderSkybox(GLuint cubemap, glm::mat4 v, glm::mat4 p);
        
        void DrawLODTerrain(GLuint heightmap, glm::mat4 m, glm::mat4 v, glm::mat4 p);
    }
}
