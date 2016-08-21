#include "WinEntry.h"
#include "Render.h"
#include <cmath>
#include "SH.h"
#include "Teapot.h"
#include "Terrain.h"
#include "Scene.h"
#include "Cube.h"

#include "Material.h"
#include "Textures.h"
#include "RenderTarget .h"
#include "Util.h"

using namespace aa;
using namespace aa::gfx;

//const double shc_two_lights[] = {
//    0.39925, 0.39925, 0.39925,
//    -0.21075, -0.21075, -0.21075,
//    0.28687, 0.28687, 0.28687,
//    0.28277, 0.28277, 0.28277,
//    -0.31530, -0.31530, -0.3153,0
//    -0.00040, -0.00040, -0.00040,
//    0.13159, 0.13159, 0.13159,
//    0.00098, 0.00098, 0.00098,
//    -0.09359, -0.09359, -0.09359
//};
//
//const double shc_grace_cathedral[] = {
//    0.7953949, 0.4405923, 0.5459412,
//    0.3981450, 0.3526911, 0.6097158,
//    -0.3424573, -0.1838151, -0.2715583,
//    -0.2944621, -0.0560606, 0.0095193,
//    -0.1123051, -0.0513088, -0.1232869,
//    -0.2645007, -0.2257996, -0.4785847,
//    -0.1569444, -0.0954703, -0.1485053,
//    0.5646247, 0.2161586, 0.1402643,
//    0.2137442, -0.0547578, -0.3061700
//};
//
//const char* filenames[6] = {
//    "src/images/right.bmp",
//    "src/images/left.bmp",
//    "src/images/bottom.bmp",
//    "src/images/top.bmp",
//    "src/images/back.bmp",
//    "src/images/front.bmp" };
//const char* filenames_test[6] = {
//    "src/images/posx.bmp",
//    "src/images/negx.bmp",
//    "src/images/posy.bmp",
//    "src/images/negy.bmp",
//    "src/images/posz.bmp",
//    "src/images/negz.bmp" };
//
//struct
//{
//    glm::mat4 p;
//    float angle;
//    float yaw;
//    float r;
//
//    glm::mat4 v()
//    {
//        glm::vec3 up(0, 1, 0);
//        glm::vec3 target(0, 0, 0);
//        glm::vec3 pos(sin(angle)*r*cos(yaw), sin(yaw)*r, cos(angle)*r*cos(yaw));
//        glm::mat4 v = glm::lookAt(pos, target, up);
//        v[0][2] *= -1.0f;
//        v[1][2] *= -1.0f;
//        v[2][2] *= -1.0f;
//        v[3][2] *= -1.0f;
//        return v;
//    }
//} camera;
//struct
//{
//    GLuint heightmap;
//    glm::mat4 m;
//} terra;
//GLuint cm, testcm, imagecm;
//GLuint testtex[6];
//RECT hrect;
//bool wireframe;
//const unsigned cmRes = 1024;
//float cm_height;
//TwBar* uibar;
//
//void DrawWorld(glm::mat4 v, glm::mat4 p)
//{
//    static int i = 0;
//
//    glClearColor(0,0,0, 1);
//
//    //if (i<2) glClearColor(1,0,0,1);
//    //else if (i<4) glClearColor(0, 1, 0, 1);
//    //else glClearColor(0, 0, 1, 1);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL);
//
//    render::RenderSkybox(imagecm, v, p);
//    //render::DrawTexturedQuad(testtex[i], 0, 0, cmRes, cmRes);
//    //::DrawLODTerrain(terrain.heightmap, terrain.m, v, p);
//
//    i = (i + 1) % 6;
//}
//
//sh::SH_t shc_my_scene;
//void fillMySH(unsigned i, unsigned size)
//{
//    sh::GenerateCoefficientsFBO(i, size, shc_my_scene);
//}
//
//void TW_CALL genSH(void * /*clientData*/)
//{
//    sh::zero(shc_my_scene);
//    render::FillCubemap(cm, cmRes, glm::vec3(0.0f, cm_height, 0.0f), &DrawWorld, &fillMySH);
//}
//
//scene::Scene sc;
//scene::Ray scray;
//scene::IntersectionPoint scip;
//void TW_CALL raytraceSceneF(void * /*clientData*/)
//{
//    scene::Intersect(sc, scray, scip);
//}
//
//void main()
//{
//    // init
//    glViewport(0, 0, hrect.right, hrect.bottom);
//    TwInit(TW_OPENGL_CORE, NULL);
//    glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);
//    GetClientRect(window::g_hWnd, &hrect);
//    wireframe = false;
//
//    // camera
//    camera.angle = 0.0f;
//    camera.yaw = 0.0f;
//    camera.r = 1.0f;
//    camera.p = glm::perspective(45.0f, (float)hrect.right / (float)hrect.bottom, 0.001f, 10.0f);
//    
//    // terrain
//    terra.m = glm::mat4(1.0f);
//    terra.m = glm::translate(terra.m, glm::vec3(-0.5f, 0.0f, -0.5f));
//    terra.heightmap = render::CreateTexture2D("src/images/terrain1.bmp");
//    cm_height = 0.6f;
//
//    // ui bar
//    uibar = TwNewBar("PRT");
//    TwAddVarRW(uibar, "Wireframe", TW_TYPE_BOOLCPP, &wireframe, "");
//    TwAddVarRW(uibar, "Radius", TW_TYPE_FLOAT, &camera.r, " min=0.01 max=1 step=0.01 ");
//    TwAddVarRW(uibar, "Height", TW_TYPE_FLOAT, &cm_height, " min=0.0 max=3 step=0.005 ");
//    TwAddButton(uibar, "Generate SH", genSH, NULL, " label='Generate SH' ");
//    TwAddSeparator(uibar, NULL, " group='Ray intersect' ");
//    TwAddVarRW(uibar, "r.o.x", TW_TYPE_FLOAT, &scray.o.x, " min=-2.0 max=2.0 step=0.005 ");
//    TwAddVarRW(uibar, "r.o.y", TW_TYPE_FLOAT, &scray.o.y, " min=-2.0 max=2.0 step=0.005 ");
//    TwAddVarRW(uibar, "r.o.z", TW_TYPE_FLOAT, &scray.o.z, " min=-2.0 max=2.0 step=0.005 ");
//    TwAddVarRW(uibar, "r.d.x", TW_TYPE_FLOAT, &scray.d.x, " min=-1.0 max=1.0 step=0.005 ");
//    TwAddVarRW(uibar, "r.d.y", TW_TYPE_FLOAT, &scray.d.y, " min=-1.0 max=1.0 step=0.005 ");
//    TwAddVarRW(uibar, "r.d.z", TW_TYPE_FLOAT, &scray.d.z, " min=-1.0 max=1.0 step=0.005 ");
//    TwAddButton(uibar, "Raytrace", raytraceSceneF, NULL, " label='Raytrace' ");
//
//    // skyboxes
//    imagecm = render::CreteTextureCubemap(filenames);
//    testcm = render::CreteTextureCubemap(filenames_test);
//    for (int i = 0; i < 6; i++) testtex[i] = render::CreateTexture2D(filenames_test[i]);
//
//    cm = render::CreateCubemapEmpty(glm::ivec2(cmRes, cmRes));
//    genSH(0);
//
//    sh::SH_t grace_catedral_sh;
//    sh::zero(grace_catedral_sh);
//    sh::make(grace_catedral_sh, shc_grace_cathedral, 9 * 3);
//
//    // scene
//    scene::Mesh cube_mesh;
//    scene::MakeMesh(cube_mesh, cube::position, cube::normals, cube::numVertices, cube::indices, cube::numIndices);
//    
//    cube_mesh.m = glm::translate(cube_mesh.m, glm::vec3(-0.1f, 0.0f, 0.0f));
//    cube_mesh.m = glm::scale(cube_mesh.m, glm::vec3(0.05f));
//    sc.push_back(cube_mesh);
//
//    scene::Mesh teapot_mesh;
//    scene::MakeMesh(teapot_mesh, (float*)teapot::position, (float*)teapot::normal, teapot::numVertices, teapot::index, teapot::numIndices);
//    teapot_mesh.m = glm::translate(teapot_mesh.m, glm::vec3(0.1f, 0.0f, 0.0f));
//    sc.push_back(teapot_mesh);
//
//    scray.o = glm::vec3(0.0f, 0.0f, 0.5f);
//    scray.d = glm::vec3(0.0f, 1.0f, 0.0f);
//    raytraceSceneF(0);
//
//    // loop
//    while (!window::windowShouldClose())
//    {
//        // input
//        {
//            input::Process();
//            // move camera left-right
//            if (input::keys[input::LEFT])   camera.angle += 0.001f;
//            if (input::keys[input::RIGHT])  camera.angle -= 0.001f;
//            // move camera up-down
//            if (input::keys[input::UP])     camera.yaw += 0.001f;
//            if (input::keys[input::DOWN])   camera.yaw -= 0.001f;
//            // zoom in-out
//            if (input::keys[input::Q])      camera.r -= 0.001f;
//            if (input::keys[input::E])      camera.r += 0.001f;
//        }
//
//        // drawing
//        {
//            //render::FillCubemap(cm, cmRes, glm::vec3(0.0f, cm_height, 0.0f), &DrawWorld);
//
//            // render
//            if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//            else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//            glViewport(0, 0, hrect.right, hrect.bottom);
//            glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            glDisable(GL_CULL_FACE);
//            glEnable(GL_DEPTH_TEST);
//            //terrain::DrawTess(terra.heightmap, terra.m, camera.v(), camera.p);
//            scene::DrawIntersectDebug(sc, camera.v(), camera.p, scip.p);
//            render::RenderSkybox(cm, camera.v(), camera.p);            
//
//            sh::DrawLatlong(grace_catedral_sh, glm::ivec2(80, 568), glm::uvec2(400, 200));
//            
//            sh::DrawLatlong(shc_my_scene, glm::ivec2(480, 568), glm::uvec2(400, 200));
//            //render::DrawCubemapAsLatlong(cm, 880, 568, 400, 200);
//            //sh::DrawProbe(shc_my_scene, 0, 0, 200);
//            //render::DrawCubemapProbe(imagecm, 680, 568,  200);
//            //render::DrawCubemapAsLatlong(testcm, 0, 368, 400, 200);
//            //render::DrawCubemapAsLatlong(imagecm, 0, 568, 400, 200);
//
//            TwDraw();
//        }
//
//        window::SwapBuffersBackend();
//    }
//
//    // cleanup
//    glDeleteTextures(1, &terra.heightmap);
//    glDeleteTextures(1, &testcm);
//    glDeleteTextures(1, &imagecm);
//    glDeleteTextures(1, &cm);
//    TwTerminate();
//}


const char* filenames[6] = {
    "src/images/right.bmp",
    "src/images/left.bmp",
    "src/images/bottom.bmp",
    "src/images/top.bmp",
    "src/images/back.bmp",
    "src/images/front.bmp" };
const char* filenames_test[6] = {
    "src/images/posx.bmp",
    "src/images/negx.bmp",
    "src/images/posy.bmp",
    "src/images/negy.bmp",
    "src/images/posz.bmp",
    "src/images/negz.bmp" };
const char img[16] = { 255, 255, 255, 255, 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255 };


void main()
{
    // init
    TwInit(TW_OPENGL_CORE, NULL);
    RECT hrect;
    GetClientRect(window::g_hWnd, &hrect);
    glViewport(0, 0, hrect.right, hrect.bottom);
    glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);

    Texture2D slika;
    slika.bmp("src/images/front.bmp");
    Texture2D testSlika(2,2);
    testSlika.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, testSlika.width, testSlika.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    Cubemap okolina;
    okolina.bmp(filenames);

    util::Tex2DViz viz2d(&testSlika);
    util::CubemapViz vizCM(&okolina);

    RT testRT(128, 128);
    testRT.use();
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, hrect.right, hrect.bottom);
    glClearColor(54.0f / 255.0f, 122.0f / 255.0f, 165.0f / 255.0f, 1.0f);

    // loop
    while (!window::windowShouldClose())
    {
        // input
        input::Process();
        if (input::keys[input::ESC]) window::quit = true;
    
        // drawing
        {
            glClear(GL_COLOR_BUFFER_BIT);

            vizCM.drawLatlong(250, 0, 500, 250);
            vizCM.drawProbe(750, 0, 250);

            viz2d.tex_uniform.tex = &testRT.color;
//            viz2d.tex_uniform.tex = &testSlika;
            viz2d.draw(0, 0, 250, 250);

            AAGFXERRCHK(glViewport(0, 0, hrect.right, hrect.bottom));
            TwDraw();
        }
    
        window::SwapBuffersBackend();
    }
    
    // cleanup
    TwTerminate();
}
