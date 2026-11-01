#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "state.h"
#include "lib/util.h"
#include "program/model0.h"

void initModel0(void)
{
}

// dSdt
float F_S(float S, float m)
{
    float a = sinf(2*S/PI);
    return -a;
}

// dXdt
float F_X(float S, float m)
{
    float r = sqrtf(S*S + m*m);
    float a = sinf(2*r/PI);
    return 1.0f;
}

float F_M(float S, float m)
{
    return 0.1f;
}

float dMdt(StateT *state_p, float dx)
{
    float S = state_p->S;
    float m = state_p->m;
    float A = F_M(S, m);

    float sigma_M = state_p->sigma_M;
    return A * gaussian(dx, sigma_M);
}
