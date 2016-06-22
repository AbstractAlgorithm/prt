#include "WinEntry.h"
#include "Render.h"
#include <cmath>
#include "SH.h"

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
    GLuint heightmap;
    glm::mat4 m;
} terrain;
GLuint cm, testcm, imagecm;
GLuint testtex[6];
RECT hrect;
bool wireframe;
const glm::ivec2 cmRes(256, 256);
float cm_height;
TwBar* uibar;

void RandomWorld()
{
    glUseProgram(0);
    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
    // +x
    glVertex3f(0.5f, 0.0f, -0.75f);
    glVertex3f(0.5f, 0.0f, 0.75f);
    glVertex3f(0.5f, 0.75f, 0.0f);
    // -x
    glVertex3f(-0.5f, 0.0f, -1.0f);
    glVertex3f(-0.5f, 0.0f, 1.0f);
    glVertex3f(-0.5f, 1.0f, 0.0f);
    // +y
    // -y
    // +z
    // -z
    glEnd();
}

void DrawWorld(glm::mat4 v, glm::mat4 p)
{
    static int i = 0;
    
    if (i<2)        glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
    else if (i<4)   glClearColor(0.0f, 0.3f, 0.0f, 1.0f);
    else            glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //aa::render::DrawTexturedQuad(testtex[i], 0, 0, cmRes.x, cmRes.y);
    //aa::render::DrawLODTerrain(terrain.heightmap, terrain.m, v, p);
    aa::render::RenderSkybox(imagecm, v, p);
    i = (i + 1) % 6;
    //RandomWorld();
    //aa::render::DrawCubemapAsLatlong(imagecm, 0, 0, 200, 200);
}

void main()
{
    // init
    glViewport(0, 0, hrect.right, hrect.bottom);
    TwInit(TW_OPENGL, NULL);
    
    glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);
    GetClientRect(aa::window::g_hWnd, &hrect);
    camera.angle = 0.0f;
    camera.yaw = 0.0f;
    camera.r = 1.0f;
    camera.p = glm::perspective(45.0f, (float)hrect.right / (float)hrect.bottom, 0.001f, 10.0f);
    
    terrain.m = glm::mat4(1.0f);
    terrain.m = glm::translate(terrain.m, glm::vec3(-0.5f, 0.0f, -0.5f));
    wireframe = false;
    terrain.heightmap = aa::render::CreateTexture2D("src/images/terrain1.bmp");
    cm_height = 0.6f;

    uibar = TwNewBar("PRT");
    TwAddVarRW(uibar, "Wireframe", TW_TYPE_BOOLCPP, &wireframe, "");
    TwAddVarRW(uibar, "Radius", TW_TYPE_FLOAT, &camera.r, " min=0.01 max=1 step=0.01 ");
    TwAddVarRW(uibar, "Height", TW_TYPE_FLOAT, &cm_height, " min=0.0 max=3 step=0.005 ");

    const char* filenames[6] = {
        "src/images/right.bmp",
        "src/images/left.bmp",
        "src/images/bottom.bmp",
        "src/images/top.bmp",
        "src/images/back.bmp",
        "src/images/front.bmp" };
    imagecm = aa::render::CreteTextureCubemap(filenames);

    const char* filenames_test[6] = {
        "src/images/posx.bmp",
        "src/images/negx.bmp",
        "src/images/posy.bmp",
        "src/images/negy.bmp",
        "src/images/posz.bmp",
        "src/images/negz.bmp" };
    testcm = aa::render::CreteTextureCubemap(filenames_test);

    for (int i = 0; i < 6; i++)
        testtex[i] = aa::render::CreateTexture2D(filenames_test[i]);

    //cm = aa::render::CreteTextureCubemap(filenames);
    cm = aa::render::CreateCubemapEmpty(cmRes);

    const float data[6] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    double* shcoeff = aa::sh::blabla((float*)data, 3);
    double koe[25];
    memcpy(koe, shcoeff, 25 * sizeof(double));
    delete[] shcoeff;


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

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
        }

        // drawing
        {

            // fill cubemap
            aa::render::FillCubemap(cm, cmRes, glm::vec3(0.0f, cm_height, 0.0f), &DrawWorld);

            // render
            if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            glViewport(0, 0, hrect.right, hrect.bottom);
            glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //aa::render::DrawLODTerrain(terrain.heightmap, terrain.m, camera.v(), camera.p);
            aa::render::RenderSkybox(imagecm, camera.v(), camera.p);
            
            aa::render::DrawCubemapAsLatlong(cm, 880, 568, 400, 200);
            //aa::render::DrawCubemapAsLatlong(testcm, 0, 368, 400, 200);
            //aa::render::DrawCubemapAsLatlong(imagecm, 0, 568, 400, 200);
            //aa::render::DrawTexturedQuad(testtex[5], 440, 280, 200, 200);
            
            TwDraw();
        }

        aa::window::SwapBuffersBackend();
    }

    // cleanup
    glDeleteTextures(1, &terrain.heightmap);
    glDeleteTextures(1, &testcm);
    glDeleteTextures(1, &imagecm);
    glDeleteTextures(1, &cm);
    TwTerminate();
}