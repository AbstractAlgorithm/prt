#include "WinEntry.h"
#include "Render.h"
#include <cmath>
#include "SH.h"

using namespace aa;

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
        glm::mat4 v = glm::lookAt(pos, target, up);
        v[0][2] *= -1.0f;
        v[1][2] *= -1.0f;
        v[2][2] *= -1.0f;
        v[3][2] *= -1.0f;
        return v;
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
const glm::ivec2 cmRes(128,128);
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
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    render::RenderSkybox(imagecm, v, p);
    render::DrawLODTerrain(terrain.heightmap, terrain.m, v, p);
}

void main()
{
    // init
    glViewport(0, 0, hrect.right, hrect.bottom);
    TwInit(TW_OPENGL, NULL);
    
    glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);
    GetClientRect(window::g_hWnd, &hrect);
    camera.angle = 0.0f;
    camera.yaw = 0.0f;
    camera.r = 1.0f;
    camera.p = glm::perspective(45.0f, (float)hrect.right / (float)hrect.bottom, 0.001f, 10.0f);
    
    terrain.m = glm::mat4(1.0f);
    terrain.m = glm::translate(terrain.m, glm::vec3(-0.5f, 0.0f, -0.5f));
    wireframe = false;
    terrain.heightmap = render::CreateTexture2D("src/images/terrain1.bmp");
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
    imagecm = render::CreteTextureCubemap(filenames);

    const char* filenames_test[6] = {
        "src/images/posx.bmp",
        "src/images/negx.bmp",
        "src/images/posy.bmp",
        "src/images/negy.bmp",
        "src/images/posz.bmp",
        "src/images/negz.bmp" };
    testcm = render::CreteTextureCubemap(filenames_test);

    for (int i = 0; i < 6; i++)
        testtex[i] = render::CreateTexture2D(filenames_test[i]);

    //cm = render::CreteTextureCubemap(filenames);
    cm = render::CreateCubemapEmpty(cmRes);

    const double vals[] = {
        0.967757057878229854,   0.976516067990363390,   0.891218272348969998,
        -0.384163503608655643,  -0.423492289131209787,  -0.425532726148547868,
        0.055906294587354334,   0.056627436881069373,   0.069969936396987467,
        0.120985157386215209,   0.119297994074027414,   0.117111965829213599,
        -0.176711633774331106,  -0.170331404095516392,  -0.151345020570876621,
        -0.124682114349692147,  -0.119340785411183953,  -0.096300354204368860,
        0.001852378550138503,   -0.032592784164597745,  -0.088204495001329680,
        0.296365482782109446,   0.281268696656263029,   0.243328223888495510,
        -0.079826665303240341,  -0.109340956251195970,  -0.157208859664677764
    };
    sh::SH_t example_sh;
    sh::make(example_sh, vals, 9*3);

    const double gcc[] = {
        .79, .44, .54,
        .39, .35, .60,
        -.34, -.18, -.27,
        -.29, -.06, .01,
        -.11, -.05, -.12,
        -.26, -.22, -.47,
        -.16, -.09, -.15,
        .56, .21, .14,
        .21, -.05, -.30
    };
    sh::SH_t grace_catedral_sh;
    sh::make(grace_catedral_sh, vals, 9 * 3);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // loop
    while (!window::windowShouldClose())
    {
        // input
        {
            input::Process();
            // move camera left-right
            if (input::keys[input::LEFT])   camera.angle += 0.01f;
            if (input::keys[input::RIGHT])  camera.angle -= 0.01f;
            // move camera up-down
            if (input::keys[input::UP])     camera.yaw += 0.01f;
            if (input::keys[input::DOWN])   camera.yaw -= 0.01f;
            // zoom in-out
            if (input::keys[input::Q])      camera.r -= 0.01f;
            if (input::keys[input::E])      camera.r += 0.01f;
        }

        // drawing
        {

            // fill cubemap
            render::FillCubemap(cm, cmRes, glm::vec3(0.0f, cm_height, 0.0f), &DrawWorld);

            // render
            if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            glViewport(0, 0, hrect.right, hrect.bottom);
            glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            render::DrawLODTerrain(terrain.heightmap, terrain.m, camera.v(), camera.p);
            render::RenderSkybox(imagecm, camera.v(), camera.p);
            
            render::DrawCubemapAsLatlong(cm, 880, 568, 400, 200);
            render::DrawCubemapProbe(imagecm, 680, 568,  200);
            //render::DrawCubemapAsLatlong(testcm, 0, 368, 400, 200);
            //render::DrawCubemapAsLatlong(imagecm, 0, 568, 400, 200);

            sh::DrawLatlong(grace_catedral_sh, glm::ivec2(480, 568), glm::uvec2(400, 200));
            
            TwDraw();
        }

        window::SwapBuffersBackend();
    }

    // cleanup
    glDeleteTextures(1, &terrain.heightmap);
    glDeleteTextures(1, &testcm);
    glDeleteTextures(1, &imagecm);
    glDeleteTextures(1, &cm);
    TwTerminate();
}