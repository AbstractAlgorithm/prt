#include <cmath>
#include <cstdint>
#include <cassert>

#define PI      3.1415926535897932384626433832795028841971693993751058
#define PI4     12.566370614359172953850573533118011536788677597500423
#define PI16    50.265482457436691815402294132472046147154710390001693
#define PI64    201.06192982974676726160917652988818458861884156000677
#define SQRT_PI 1.7724538509055160272981674833411451827975494561223871
#define SQRT_2  1.4142135623730950488016887242096980785696718753769480

// Based upon the "Spherical Harmonics Lighting: The Gritty Details" by Robin Green"

// -----------------------------------------------------------------------------
// 1D
// -----------------------------------------------------------------------------

// P{m,l}
// l - band index  in [0,oo]
// m - takes value in [0, l]

// 1. (l-m)P{m,l} = x * (2l-1) * P{m,l-1} - (l+m-1) * P{m,l-2}
// 2. P{m,m} = (-1)^m * (2m-1)!! * (1-x*x)^(m/2)
// 3. P{m,m+1} = x(2m+1) * P{m,m}

double P(uint32_t l, uint32_t m, double x)
{
    double pmm = 1.0;
    if (m > 0)
    {
        double somx2 = sqrt((1.0 - x)*(1.0 + x));
        double fact = 1.0;
        for (uint8_t i = 1; i <= m; ++i)
        {
            pmm *= (-fact) * somx2;
            fact += 2.0;
        }
    }
    if (l == m) return pmm;
    double pmmp1 = x*(2.0*m + 1.0)*pmm;
    if (l == m + 1) return pmmp1;
    double pll = 0.0;
    for (uint8_t ll = m + 2; ll <= l; ++ll)
    {
        pll = ((2.0*ll + 1.0)*x*pmmp1 - (ll + m - 1.0)*pmm) / (ll - m);
        pmm = pmmp1;
        pmmp1 = pll;
    }
    return pll;
}

// -----------------------------------------------------------------------------
// (Real) Spherical Harmonics
// -----------------------------------------------------------------------------

// sin(theta) * cos(phi)    => x
// sin(theta) * sin(phi)    => y
// cos(theta)               => z

// Y{m,l}
// l - band index  in [0,oo]
// m - takes value in [-l,l]

// Y{m,l,theta,phi}
// m = 0: K{0,l} * P{0,l,cos(theta)}
// m < 0: sqrt(2) * K{m,l} * sin(-m*phi) * P{-m,l,cos(theta)}
// m > 0: sqrt(2) * K{m,l} * cos( m*phi) * P{ m,l,cos(theta)}
// where K{m,l} = sqrt( (2*l+1)/(4*pi) * (l-|m|)!/(l+m)! )

// Y{m,l} = Y{i} where i = l(l+1) + m

uint32_t factorial(uint32_t n)
{
    const uint32_t table[13] =
    {
        1,
        1,
        2,
        6,
        24,
        120,
        720,
        5040,
        40320,
        362880,
        3628800,
        39916800,
        479001600
    };
    assert(n < 13);
    return table[n];

    /*uint64_t res = 1;
    for (uint64_t i = 2; i <= n; ++n)
        res *= i;
    return res;*/
}


double K(uint32_t l, uint32_t m)
{
    double temp = ((2.0*l + 1.0) * factorial(l - m)) / (PI4*factorial(l + m));
    return sqrt(temp);
}

/*
Returns a point sample of a SH basis function
l:      [0..N] band 
m:      [-l..l]
theta:  [0..Pi]
phi:    [0..2*Pi]
*/
double SH(uint32_t l, uint32_t m, double theta, double phi)
{
    if (m == 0)     return K(l, 0) *P(l, m, cos(theta));
    else if (m < 0) return SQRT_2 * K(l, m) * sin(-m*phi) * P(l, -m, cos(theta));
    else            return SQRT_2 * K(l, m) * cos( m*phi) * P(l,  m, cos(theta));
}

// c{i} = int{S} [ f(s) * Y{i,s} ] 
// _f(s) = sum{i=0,n*n} [c{i} * Y{i}]

