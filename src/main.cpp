#include "WinEntry.h"
#include "Render.h"

void main()
{
    // TwInit(TW_OPENGL, NULL);
    glClearColor(1, 0, 0, 1);

    RECT hrect;
    glm::vec3 cameraPos(0, 0, -1);
    glm::vec3 target(0, 0, 0);
    glm::vec3 up(0, 1, 0);
    glm::mat4 m, v, p;
    GLuint heightmap;

    
    GetClientRect(aa::window::g_hWnd, &hrect);
    heightmap = aa::render::CreateTexture2D("src/images/terrain1.bmp");
    m = glm::mat4(1.0f);
    v = glm::lookAt(cameraPos, target, up);
    p = glm::perspective(45.0f, (float)hrect.right / (float)hrect.bottom, 0.1f, 100.0f);

    while (!aa::window::windowShouldClose())
    {
        aa::input::Process();
        glClear(GL_COLOR_BUFFER_BIT);
        aa::render::DrawTexturedQuad(heightmap, 0, 0, 200, 200);
        aa::render::DrawLODTerrain(heightmap, m, v, p);
        // TwDraw();
        aa::window::SwapBuffersBackend();
    }

    glDeleteTextures(1, &heightmap);
    // TwTerminate();
}