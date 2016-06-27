#include "WinEntry.h"
#include "Render.h"
#include <cmath>
#include "SH.h"
#include "teapot.h"

using namespace aa;

const double shc_two_lights[] = {
    0.39925, 0.39925, 0.39925,
    -0.21075, -0.21075, -0.21075,
    0.28687, 0.28687, 0.28687,
    0.28277, 0.28277, 0.28277,
    -0.31530, -0.31530, -0.31530,
    -0.00040, -0.00040, -0.00040,
    0.13159, 0.13159, 0.13159,
    0.00098, 0.00098, 0.00098,
    -0.09359, -0.09359, -0.09359
};

const double shc_grace_cathedral[] = {
    0.7953949, 0.4405923, 0.5459412,
    0.3981450, 0.3526911, 0.6097158,
    -0.3424573, -0.1838151, -0.2715583,
    -0.2944621, -0.0560606, 0.0095193,
    -0.1123051, -0.0513088, -0.1232869,
    -0.2645007, -0.2257996, -0.4785847,
    -0.1569444, -0.0954703, -0.1485053,
    0.5646247, 0.2161586, 0.1402643,
    0.2137442, -0.0547578, -0.3061700
};

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
const unsigned cmRes = 256;
float cm_height;
TwBar* uibar;

namespace Teapot
{
    struct
    {
        GLuint vao, vbo_pos, vbo_nor, vbo_uv, ibo;
        GLint uloc_m, uloc_v, uloc_p;
        GLuint program;
        glm::mat4 modelMat;
    } obj;

    void Init()
    {
        glGenVertexArrays(1, &obj.vao);
        glBindVertexArray(obj.vao);

        // pos
        glGenBuffers(1, &obj.vbo_pos);
        glBindBuffer(GL_ARRAY_BUFFER, obj.vbo_pos);
        glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), position, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // nor
        glGenBuffers(1, &obj.vbo_nor);
        glBindBuffer(GL_ARRAY_BUFFER, obj.vbo_nor);
        glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), normal, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // indices
        glGenBuffers(1, &obj.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(int), index, GL_STATIC_DRAW);

        glBindVertexArray(0);

        // shader
        GLuint program = glCreateProgram();
        {
            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            static const char* vs_shdr = GLSLify(330,
                in vec3 iPos;
            in vec3 iNormal;
            uniform mat4 uModelMat;
            uniform mat4 uViewMat;
            uniform mat4 uProjMat;
            out vec3 normal;

            void main()
            {
                gl_Position = uProjMat * uViewMat * uModelMat *vec4(iPos, 1.0);
                normal = iNormal;
            }
            );
            glShaderSource(vs, 1, &vs_shdr, NULL);
            glCompileShader(vs);
            glAttachShader(program, vs);

            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fs_shdr = GLSLify(330,
                out vec4 fragColor;
            in vec3 normal;

            void main()
            {
                fragColor = vec4(normal, 1.0);
            }
            );
            glShaderSource(fs, 1, &fs_shdr, NULL);
            glCompileShader(fs);
            glAttachShader(program, fs);

            glLinkProgram(program);
            glDeleteShader(vs);
            glDeleteShader(fs);
            {
                printf("Teapot shader info:\n");
                int infologLen = 0;
                int charsWritten = 0;
                GLchar *infoLog = NULL;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);
                if (infologLen > 0)
                {
                    infoLog = (GLchar*)malloc(infologLen);
                    if (infoLog == NULL)
                        return;
                    glGetProgramInfoLog(program, infologLen, &charsWritten, infoLog);
                }
                printf("%s\n", infoLog);
                delete[] infoLog;
            }
        }
        obj.uloc_m = glGetUniformLocation(program, "uModelMat");
        obj.uloc_v = glGetUniformLocation(program, "uViewMat");
        obj.uloc_p = glGetUniformLocation(program, "uProjMat");
        obj.program = program;
    }
    void Draw(glm::mat4 v, glm::mat4 p)
    {
        glUseProgram(obj.program);

        glUniformMatrix4fv(obj.uloc_m, 1, GL_FALSE, glm::value_ptr(obj.modelMat));
        glUniformMatrix4fv(obj.uloc_v, 1, GL_FALSE, glm::value_ptr(v));
        glUniformMatrix4fv(obj.uloc_p, 1, GL_FALSE, glm::value_ptr(p));

        glBindVertexArray(obj.vao);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    }
    void Destroy()
    {
        glBindVertexArray(0);
        glDeleteBuffers(1, &obj.vbo_pos);
        glDeleteBuffers(1, &obj.ibo);
        glDeleteBuffers(1, &obj.vbo_nor);
        glDeleteVertexArrays(1, &obj.vao);
        obj.vbo_pos = 0;
        obj.vbo_nor = 0;
        obj.ibo = 0;
        obj.vao = 0;

        glDeleteProgram(obj.program);
        obj.program = 0;
    }
}




void DrawWorld(glm::mat4 v, glm::mat4 p)
{
    static int i = 0;

    glClearColor(1,1,1, 1);

    if (i<2) glClearColor(1,0,0,1);
    else if (i<4) glClearColor(0, 1, 0, 1);
    else glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    render::RenderSkybox(imagecm, v, p);
    //render::DrawTexturedQuad(testtex[i], 0, 0, cmRes, cmRes);
    render::DrawLODTerrain(terrain.heightmap, terrain.m, v, p);

    i = (i + 1) % 6;
}

sh::SH_t shc_my_scene;
void fillMySH(unsigned i, unsigned size)
{
    sh::GenerateCoefficientsFBO(i, size, shc_my_scene);
}

void TW_CALL genSH(void * /*clientData*/)
{
    sh::zero(shc_my_scene);
    render::FillCubemap(cm, cmRes, glm::vec3(0.0f, cm_height, 0.0f), &DrawWorld, &fillMySH);
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
    TwAddButton(uibar, "Generate SH", genSH, NULL, " label='Generate SH' ");

    imagecm = render::CreteTextureCubemap(filenames);
    testcm = render::CreteTextureCubemap(filenames_test);
    for (int i = 0; i < 6; i++) testtex[i] = render::CreateTexture2D(filenames_test[i]);

    cm = render::CreateCubemapEmpty(glm::ivec2(cmRes, cmRes));
    genSH(0);

    sh::SH_t grace_catedral_sh;
    sh::zero(grace_catedral_sh);
    sh::make(grace_catedral_sh, shc_grace_cathedral, 9 * 3);

    Teapot::Init();
    Teapot::obj.modelMat = glm::scale(Teapot::obj.modelMat, glm::vec3(2, 2, 2));

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

            Teapot::Draw(camera.v(), camera.p);

            //sh::DrawLatlong(grace_catedral_sh, glm::ivec2(80, 568), glm::uvec2(400, 200));
            
            sh::DrawLatlong(shc_my_scene, glm::ivec2(480, 568), glm::uvec2(400, 200));
            render::DrawCubemapAsLatlong(cm, 880, 568, 400, 200);
            //sh::DrawProbe(shc_my_scene, 0, 0, 200);
            //render::DrawCubemapProbe(imagecm, 680, 568,  200);
            //render::DrawCubemapAsLatlong(testcm, 0, 368, 400, 200);
            //render::DrawCubemapAsLatlong(imagecm, 0, 568, 400, 200);

            TwDraw();
        }

        window::SwapBuffersBackend();
    }

    // cleanup
    Teapot::Destroy();
    glDeleteTextures(1, &terrain.heightmap);
    glDeleteTextures(1, &testcm);
    glDeleteTextures(1, &imagecm);
    glDeleteTextures(1, &cm);
    TwTerminate();
}