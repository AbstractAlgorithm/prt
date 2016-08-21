#pragma once

#include "WinEntry.h"
#include "Textures.h"

namespace aa
{
    namespace gfx
    {
        struct Shader
        {
            GLuint shdr;
            GLenum ty;
            Shader(GLenum _ty);
            void src(const char* source);
            void log();
            ~Shader();
        };
        struct VertexShader : public Shader { VertexShader(); };
        struct TessCtrlShader : public Shader { TessCtrlShader(); };
        struct TessEvalShader : public Shader { TessEvalShader(); };
        struct GeometryShader : public Shader { GeometryShader(); };
        struct FragmentShader : public Shader { FragmentShader(); };
        struct ComputeShader : public Shader { ComputeShader(); };

        struct Program
        {
            GLuint pgm;
            Program();
            Program(Shader s1);
            Program(Shader s1, Shader s2);
            Program(Shader s1, Shader s2, Shader s3);
            Program(Shader s1, Shader s2, Shader s3, Shader s4);
            Program(Shader s1, Shader s2, Shader s3, Shader s4, Shader s5);
            ~Program();
            void log();
            void use();
        };
        typedef Program Material;

        struct float_u
        {
            GLint loc;
            GLfloat val;
            float_u(Material& m, const char* name);
            void bind();
        };
        struct vec2_u
        {
            GLint loc;
            glm::vec2 val;
            vec2_u(Material& m, const char* name);
            void bind();
        };
        struct vec3_u
        {
            GLint loc;
            glm::vec3 val;
            vec3_u(Material& m, const char* name);
            void bind();
        };
        struct vec4_u
        {
            GLint loc;
            glm::vec4 val;
            vec4_u(Material& m, const char* name);
            void bind();
        };
        struct int_u
        {
            GLint loc;
            GLint val;
            int_u(Material& m, const char* name);
            void bind();
        };
        struct ivec2_u
        {
            GLint loc;
            glm::ivec2 val;
            ivec2_u(Material& m, const char* name);
            void bind();
        };
        struct ivec3_u
        {
            GLint loc;
            glm::ivec3 val;
            ivec3_u(Material& m, const char* name);
            void bind();
        };
        struct ivec4_u
        {
            GLint loc;
            glm::ivec4 val;
            ivec4_u(Material& m, const char* name);
            void bind();
        };
        struct uint_u
        {
            GLint loc;
            GLuint val;
            uint_u(Material& m, const char* name);
            void bind();
        };
        struct uvec2_u
        {
            GLint loc;
            glm::uvec2 val;
            uvec2_u(Material& m, const char* name);
            void bind();
        };
        struct uvec3_u
        {
            GLint loc;
            glm::uvec3 val;
            uvec3_u(Material& m, const char* name);
            void bind();
        };
        struct uvec4_u
        {
            GLint loc;
            glm::uvec4 val;
            uvec4_u(Material& m, const char* name);
            void bind();
        };
        struct tex2d_u
        {
            GLint loc;
            GLuint slot;
            Texture2D* tex;
            tex2d_u();
            tex2d_u(Material& m, const char* name, Texture2D* tx = 0, GLuint slt = 0);
            void bind();
        };
        struct texcm_u
        {
            GLint loc;
            GLuint slot;
            Cubemap* tex;
            texcm_u();
            texcm_u(Material& m, const char* name, Cubemap* tx = 0, GLuint slt = 0);
            void bind();
        };
    }
}
