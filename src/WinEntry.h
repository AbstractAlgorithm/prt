#pragma once

// <options> -------------------------------------------------------------------
#define USE_CONSOLE_BACKEND
// </options> ------------------------------------------------------------------

// commonly used keys
namespace aa
{
    struct input
    {
        enum Key
        {
            // common
            ESC     = 0x1B, BACK    = 0x08, TAB     = 0x09, ENTER   = 0x0D,
            SPACE   = 0x20, PAUSE   = 0x13, CAPS    = 0x14, INSERT  = 0x2D,
            DELETE  = 0x2E, SHIFT   = 0x10, CTRL    = 0x11, ALT     = 0x12,
            LSHIFT  = 0xA0, RSHIFT  = 0xA1, LCTRL   = 0xA2, RCTRL   = 0xA3,
            LALT    = 0xA4, RALT    = 0xA5,
            // arrow keys
            LEFT    = 0x25, UP      = 0x26, RIGHT   = 0x27, DOWN    = 0x28,
            // numeric
            N0 = 0x30, N1 = 0x31, N2 = 0x32, N3 = 0x33, N4 = 0x34,
            N5 = 0x35, N6 = 0x36, N7 = 0x37, N8 = 0x38, N9 = 0x39,
            // letters
            A = 0x41, B = 0x42, C = 0x43, D = 0x44, E = 0x45, F = 0x46,
            G = 0x47, H = 0x48, I = 0x49, J = 0x4A, K = 0x4B, L = 0x4C,
            M = 0x4D, N = 0x4E, O = 0x4F, P = 0x50, Q = 0x51, R = 0x52,
            S = 0x53, T = 0x54, U = 0x55, V = 0x56, W = 0x57, X = 0x58,
            Y = 0x59, Z = 0x5A,
            // function keys
            F1 = 0x70, F2 = 0x71, F3 = 0x72, F4  = 0x73, F5  = 0x74, F6  = 0x75,
            F7 = 0x76, F8 = 0x77, F9 = 0x78, F10 = 0x79, F11 = 0x7A, F12 = 0x7B,
            // mouse
            MOUSE_LEFT = 0x0001, MOUSE_MIDDLE = 0x0010, MOUSE_RIGHT = 0x0002
        };
        static void(*mouseMoveF)(int, int, unsigned int, unsigned int);
        static void(*mouseDownF)(int);
        static void(*mouseUpF)(int);
        static void(*mouseDoubleF)(int);
        static void(*keyDownF)(unsigned int);
        static void(*keyUpF)(unsigned int);
        static void(*resizeF)(unsigned int, unsigned int);
        static bool keys[256];
        static unsigned int mx, my;
        static void Process();
    };
}

// loading graphics backend libraries
#include <Windows.h>
#include <stdio.h>
// GLEW
#include <glew/glew.h>
// GLI
#include <gli/gli.hpp>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// NVTX
// #include <nvtx/nvToolsExt.h>
// ANTTWEAKBAR
 #include <anttweakbar/AntTweakBar.h>

namespace aa
{
    namespace window
    {
        extern HINSTANCE g_hInst;
        extern HWND g_hWnd;
        extern HGLRC g_hRC;
        extern HDC g_hDC;
        extern bool quit;

        HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
        LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
        void CleanupWindow();
        bool windowShouldClose();

        DWORD WINAPI BackendProc(void* lp);
        HRESULT InitBackend();
        void CleanupBackend();
        void SwapBuffersBackend();
    }
}

#define AAGFXERRCHK(glFn) \
do { \
glFn; \
GLenum err = glGetError(); \
const GLubyte* errmsg = gluErrorString(err); \
if (err != GL_NO_ERROR)    \
    printf("ERROR: 0x%x (%s)\n%s : %d\n", err, errmsg, __FILE__, __LINE__); \
} while (0)


void main();
