#include "WinEntry.h"

void main()
{
    // TwInit(TW_OPENGL, NULL);
    glClearColor(1, 0, 0, 1);

    while (!aa::window::windowShouldClose())
    {
        aa::input::Process();
        glClear(GL_COLOR_BUFFER_BIT);
        // TwDraw();
        aa::window::SwapBuffersBackend();
    }

    // TwTerminate();
}