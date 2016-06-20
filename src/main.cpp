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
struct
{
    bool wPrev, wShow;
    GLuint heightmap;
    glm::mat4 m;
} terrain;
GLuint cm;
RECT hrect;

void DrawWorld(glm::mat4 v, glm::mat4 p)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    aa::render::DrawLODTerrain(terrain.heightmap, terrain.m, v, p, terrain.wShow);
    //aa::render::DrawCubemapAsLatlong(cm, 40, 280, 400, 200);
    //aa::render::DrawTexturedQuad(terrain.heightmap, 440, 280, 200, 200);
}

void main()
{
    // init
    // TwInit(TW_OPENGL, NULL);
    glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);
    
    GetClientRect(aa::window::g_hWnd, &hrect);
    glViewport(0, 0, hrect.right, hrect.bottom);
    camera.angle = 0.0f;
    camera.yaw = 0.0f;
    camera.r = 1.0f;
    camera.p = glm::perspective(45.0f, (float)hrect.right / (float)hrect.bottom, 0.001f, 10.0f);
    
    terrain.m = glm::mat4(1.0f);
    terrain.m = glm::translate(terrain.m, glm::vec3(-0.5f, 0.0f, -0.5f));
    terrain.wPrev = false;
    terrain.wShow = false;
    terrain.heightmap = aa::render::CreateTexture2D("src/images/terrain1.bmp");

    const char* filenames[6] = {
        "src/images/right.bmp",
        "src/images/left.bmp",
        "src/images/bottom.bmp",
        "src/images/top.bmp",
        "src/images/back.bmp",
        "src/images/front.bmp" };

    //cm = aa::render::CreteTextureCubemap(filenames);
    glm::ivec2 cmRes(256, 256);
    cm = aa::render::CreateCubemapEmpty(cmRes);

    // loop
    while (!aa::window::windowShouldClose())
    {
        // input
        {
            aa::input::Process();
            // move camera left-right
            if (aa::input::keys[aa::input::LEFT])   camera.angle += 0.01f;
            if (aa::input::keys[aa::input::RIGHT])  camera.angle -= 0.01f;
            // move camera up-down
            if (aa::input::keys[aa::input::UP])     camera.yaw += 0.01f;
            if (aa::input::keys[aa::input::DOWN])   camera.yaw -= 0.01f;
            // zoom in-out
            if (aa::input::keys[aa::input::Q])      camera.r -= 0.01f;
            if (aa::input::keys[aa::input::E])      camera.r += 0.01f;
            // toggle wireframe
            if (!aa::input::keys[aa::input::W] && terrain.wPrev) terrain.wShow = !terrain.wShow;
            terrain.wPrev = aa::input::keys[aa::input::W];
        }

        // drawing
        {
            aa::render::RenderCubemap(cm, cmRes, glm::vec3(0.0f,0.5f, 0.0f), &DrawWorld);
            glViewport(0, 0, hrect.right, hrect.bottom);
            aa::render::DrawCubemapAsLatlong(cm, 0, 0, 640, 480);
            //DrawWorld(camera.v(), camera.p);
            // TwDraw();
        }

        aa::window::SwapBuffersBackend();
    }

    // cleanup
    glDeleteTextures(1, &terrain.heightmap);
    // TwTerminate();
}