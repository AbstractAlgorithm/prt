#pragma once

#include "WinEntry.h"
#include <cmath>
#include <cstdint>
#include <cassert>

#define PI      3.1415926535897932384626433832795028841971693993751058
#define PI4     12.566370614359172953850573533118011536788677597500423
#define PI16    50.265482457436691815402294132472046147154710390001693
#define PI64    201.06192982974676726160917652988818458861884156000677
#define SQRT_PI 1.7724538509055160272981674833411451827975494561223871
#define SQRT_2  1.4142135623730950488016887242096980785696718753769480

/*
Resources:
Solid angle of a rectangular plate: http://www.mpia.de/~mathar/public/mathar20051002.pdf
Cubemap texel solid angle: http://www.rorydriscoll.com/2012/01/15/cubemap-texel-solid-angle/
"Spherical Harmonics Lighting: The Gritty Details" by Robin Green"
CUbemap Filtering Tool: https://github.com/dariomanesku/cmft
https://github.com/bkaradzic/bgfx/blob/master/examples/28-wireframe/fs_wf_mesh.sc
*/

namespace aa
{
    namespace sh
    {
        double* blabla(float* data, uint8_t bands);

        template<uint8_t bandsCnt>
        struct SH_t
        {
            double coeff[bandsCnt*bandsCnt];

            SH_t()
            {
                for (uint8_t i = 0; i < bandsCnt; i++)
                    coeff[i] = 0.0;
            }

            void zero()
            {
                for (uint8_t i = 0; i < bandsCnt; i++)
                    coeff[i] = 0.0;
            }

            SH_t& operator+=(const SH_t& rhs)
            {
                for (uint8_t i = 0; i < bandsCnt; i++)
                    coeff[i] += rhs.coeff[i];
                return *this;
            }
        };

        // ---------------------------------------------------------------------

        struct SHGenerator
        {
            GLuint program;

            SHGenerator();
            ~SHGenerator();

            template<uint8_t bcnt>
            void gen(GLuint cubemap, glm::ivec2 res, aa::sh::SH_t<bcnt>& sh)
            {
                // setup uniforms
                // bind cubemap
                // declare workgroup size depenging on the size
                // allocate memory for the results
                // bind the memory for the result to the shader
                // invoke shader (dispatch)

                /*for (uint8_t fi = 0; fi < 6; fi++)
                {
                    GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + fi;
                    float* positions = (vmath::vec4 *)
                        glMapBufferRange(GL_ARRAY_BUFFER,
                        0,
                        PARTICLE_COUNT * sizeof(vmath::vec4),
                        GL_MAP_WRITE_BIT |
                        GL_MAP_INVALIDATE_BUFFER_BIT);
                }*/

            }

            template<uint8_t bcnt>
            void evalSHBasis(SH_t<bcnt>& sh, glm::vec3 dir)
            {

            }

            template<uint8_t bcnt>
            void processFace(glm::uvec2 res, aa::sh::SH_t<bcnt>& sh)
            {
                byte* data = new byte[res.x*res.y * 3];
                glReadPixels(0, 0, res.x, res.y, GL_RGB, GL_UNSIGNED_BYTE, data);

                sh.zero();
                SH_t<bcnt> texel_sh;

                // foreach texel
                for (unsigned i = 0; i < res.x; i++)
                {
                    for (unsigned j = res.y - 1; j >= 0; j++)
                    {
                        // read colors
                        byte r = data[(j*res.x + i) * 3 + 0];
                        byte g = data[(j*res.x + i) * 3 + 1];
                        byte b = data[(j*res.x + i) * 3 + 2];

                        // calc texel's vector
                        glm::vec3 dir;

                        // calc solid angle of the texel
                        double solidAngleWeight = 1.0;

                        // eval SH basis
                        evalSHBasis(texel_sh, dir);

                        // add to total SH coeffs
                        sh += texel_sh;
                    }
                }
            }
        };

        template<uint8_t bcnt>
        void GenerateCoefficients(GLuint cubemap, glm::ivec2 res, SH_t<bcnt>& sh)
        {
            static SHGenerator shgen;
            return shgen.gen(cubemap, res, sh);
        }

        // ---------------------------------------------------------------------

        struct SHPainter
        {
            GLuint vao, vbo, program;
            GLint uloc_shc, uloc_res, uloc_dim;

            SHPainter();
            ~SHPainter();

            template<uint8_t bcnt>
            void draw(aa::sh::SH_t<bcnt> sh, glm::ivec2 pos, glm::uvec2 dim)
            {
                assert(bcnt < 6);
                glUseProgram(program);

                glDisable(GL_CULL_FACE);
                glDisable(GL_DEPTH_TEST);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                int dims[4];
                glGetIntegerv(GL_VIEWPORT, dims);
                glUniform2f(uloc_res, dims[2], dims[3]);
                glUniform4f(uloc_dim, (float)pos.x, (float)pos.y, (float)dim.x, (float)dim.y);
                GLfloat shc_[75];
                for (int i = 0; i < 25; i++)
                {
                    shc_[3 * i] = 0.0f;
                    shc_[3 * i + 1] = 0.0f;
                    shc_[3 * i + 2] = 0.0f;
                }
                shc_[0] = 0.967757057878229854f, shc_[1] = 0.976516067990363390, shc_[2] = 0.891218272348969998;
                shc_[3] = -0.384163503608655643, shc_[4] = -0.423492289131209787, shc_[5] = -0.425532726148547868; /* Band 1 */
                shc_[6] = 0.055906294587354334, shc_[7] = 0.056627436881069373, shc_[8] = 0.069969936396987467;
                shc_[9] = 0.120985157386215209, shc_[10] = 0.119297994074027414, shc_[11] = 0.117111965829213599;
                shc_[12] = -0.176711633774331106, shc_[13] = -0.170331404095516392, shc_[14] = -0.151345020570876621; /* Band 2 */
                shc_[15] = -0.124682114349692147, shc_[16] = -0.119340785411183953, shc_[17] = -0.096300354204368860;
                shc_[18] = 0.001852378550138503, shc_[19] = -0.032592784164597745, shc_[20] = -0.088204495001329680;
                shc_[21] = 0.296365482782109446, shc_[22] = 0.281268696656263029, shc_[23] = 0.243328223888495510;
                shc_[24] = -0.079826665303240341, shc_[25] = -0.109340956251195970, shc_[26] = -0.157208859664677764;
                glUniform3fv(uloc_shc, 25, shc_);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        };

        template<uint8_t bcnt>
        void DrawLatlong(aa::sh::SH_t<bcnt> sh, glm::ivec2 pos, glm::uvec2 dim)
        {
            static SHPainter shp;
            shp.draw(sh, pos, dim);
        }
    }
}

