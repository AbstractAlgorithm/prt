#pragma once

#include "WinEntry.h"
#include <cmath>
#include <cstdint>
#include <cassert>
#include <vector>

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
        // ---------------------------------------=-=- Spherical Harmonics -=-=-
        static const unsigned SH_BANDS = 5;
        static const unsigned SH_CHANNELS = 3;
        static const unsigned RED = 0;
        static const unsigned GREEN = 1;
        static const unsigned BLUE = 2;

        typedef double SH_t[SH_CHANNELS][SH_BANDS*SH_BANDS];

        void make(SH_t& sh, const double* values, int coeffsCnt);
        void zero(SH_t& sh);
        void copy(SH_t& a, const SH_t& b);
        void add(SH_t& a, const SH_t& b);
        void add(SH_t& c, const SH_t& a, const SH_t& b);
        void sub(SH_t& a, const SH_t& b);
        void sub(SH_t& c, const SH_t& a, const SH_t& b);
        void mul(SH_t& a, double s);
        void div(SH_t& a, double s);

        double Y(int l, int m, double theta, double phi);

        // --------------------------------------------=-=- cool functions -=-=-

        void GenerateCoefficients(GLuint cubemap, unsigned size, SH_t& sh);
        void DrawLatlong(SH_t sh, glm::ivec2 pos, glm::uvec2 dim);
    }
}

