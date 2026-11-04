#include <math.h>

#include "lib/util.h"
#include "models/model_1d/velocity_constant.h"

static void init(Model1DDataT *data)
{
}

static float F_S(const Model1DDataT *data, float S, float m)
{
    float a = sinf(2*S/PI);
    return -a;
}

static float F_X(const Model1DDataT *data, float S, float m)
{
    float r = sqrtf(S*S + m*m);
    float a = sinf(2*r/PI);
    return 1.0f;
}

static float F_M(const Model1DDataT *data, float S, float m)
{
    return 0.1f;
}

const VelocityFieldT VelocityConstant = {
    .init = init,
    .F_S  = F_S,
    .F_X  = F_X,
    .F_M  = F_M,
};
