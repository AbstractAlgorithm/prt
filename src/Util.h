#pragma once

#include "WinEntry.h"
#include "Texture2D.h"
#include "Material.h"
#include <cmath>
#include <cstdint>
#include <cassert>
#include <vector>

#define AAGFXERRCHK(glFn) \
do { \
glFn; \
GLenum err = glGetError(); \
const GLubyte* errmsg = gluErrorString(err); \
if (err != GL_NO_ERROR)    \
    printf("ERROR: 0x%x (%s)\n%s : %d\n", err, errmsg, __FILE__, __LINE__); \
} while (0)

using namespace aa::gfx;

namespace aa
{
    namespace util
    {
        GLuint setupQuadProgram(const char* fragShdrSrc);
        void drawQuad(GLuint pgm, int x, int y, int w, int h);

        struct Tex2DViz
        {
            static const char* fs;
            Material mat;
            tex2d_u tex_uniform;
            Tex2DViz(Texture2D* tex = 0);
            void draw(int x, int y, int w, int h);
        };
        struct CubemapViz
        {
            static const char* fs;
            Material mat;
            texcm_u tex_uniform;
            CubemapViz(Cubemap* tex = 0);
            void draw(int x, int y, int w, int h);
        };
    }
}
