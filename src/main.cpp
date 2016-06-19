#include "WinEntry.h"
#include "Render.h"

void main()
{
    // TwInit(TW_OPENGL, NULL);
    glClearColor(1, 0, 0, 1);

    GLuint t = aa::render::CreateTexture2D("src/images/terrain1.bmp");

    while (!aa::window::windowShouldClose())
    {
        aa::input::Process();
        glClear(GL_COLOR_BUFFER_BIT);
        aa::render::DrawTexturedQuad(t, 0, 0, 200, 200);
        // TwDraw();
        aa::window::SwapBuffersBackend();
    }

    // TwTerminate();
}