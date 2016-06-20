#include "WinEntry.h"
#include "Render.h"
#include <cmath>

struct
{
    
    
    glm::mat4 p;
    float angle;
    float yaw;
    float r;

    glm::mat4 v()
    {
        glm::vec3 up(0, 1, 0);
        glm::vec3 target(0, 0, 0);
        glm::vec3 pos(cos(angle)*r*cos(yaw), sin(yaw)*r, sin(angle)*r*cos(yaw));
        return glm::lookAt(pos, target, up);
    }
} camera;

void main()
{
    // init
    // TwInit(TW_OPENGL, NULL);
    glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);
    RECT hrect;
    GetClientRect(aa::window::g_hWnd, &hrect);
    glViewport(0, 0, hrect.right, hrect.bottom);
    camera.angle = 0.0f;
    camera.yaw = 0.0f;
    camera.r = 1.0f;
    camera.p = glm::perspective(45.0f, (float)hrect.right / (float)hrect.bottom, 0.001f, 10.0f);
    glm::mat4 m;
    m = glm::mat4(1.0f);
    m = glm::translate(m, glm::vec3(-0.5f, 0.0f, -0.5f));
    bool wPrev = false, wShow = false;
    GLuint heightmap = aa::render::CreateTexture2D("src/images/terrain1.bmp");

    // loop
    while (!aa::window::windowShouldClose())
    {
        // input
        aa::input::Process();
        if (aa::input::keys[aa::input::LEFT])
            camera.angle += 0.01f;
        if (aa::input::keys[aa::input::RIGHT])
            camera.angle -= 0.01f;
        if (aa::input::keys[aa::input::E])
            camera.r -= 0.01f;
        if (aa::input::keys[aa::input::R])
            camera.r += 0.01f;
        if (aa::input::keys[aa::input::UP])
            camera.yaw += 0.01f;
        if (aa::input::keys[aa::input::DOWN])
            camera.yaw -= 0.01f;
        if (!aa::input::keys[aa::input::W] && wPrev)
            wShow = !wShow;
        wPrev = aa::input::keys[aa::input::W];

        // drawing
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        aa::render::DrawLODTerrain(heightmap, m, camera.v(), camera.p, wShow);
        // aa::render::DrawTexturedQuad(heightmap, 440, 280, 200, 200);
        // TwDraw();

        aa::window::SwapBuffersBackend();
    }

    // cleanup
    glDeleteTextures(1, &heightmap);
    // TwTerminate();
}