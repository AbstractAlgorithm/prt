#pragma once

#include "WinEntry.h"
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

namespace aa
{
    namespace util
    {
        GLuint setupQuadProgram(const char* fragShdrSrc);
        void drawQuad(GLuint pgm, int x, int y, int w, int h);
    }
}
