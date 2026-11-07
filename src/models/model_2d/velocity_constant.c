#include <complex.h>

#include "models/model_2d/velocity_constant.h"

#define ORBIT_RADIUS 0.5f
#define ATTRACTION   5.0f

static void init(Model2DDataT *data)
{
}

static float complex F_S(const Model2DDataT *data, float complex S, float m, float theta)
{
    float complex target = ORBIT_RADIUS * cexpf(I * theta);
    return ATTRACTION * (target - S);
}

static float F_X(const Model2DDataT *data, float complex S, float m, float theta)
{
    return 0.1f;
}

static float F_M(const Model2DDataT *data, float complex S, float m, float theta)
{
    return 0.0f;
}

const VelocityField2DT model2d_VelocityConstant = {
    .init = init,
    .F_S  = F_S,
    .F_X  = F_X,
    .F_M  = F_M,
};
