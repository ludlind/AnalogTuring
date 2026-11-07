#include <math.h>
#include <complex.h>

#include "lib/util.h"
#include "models/model_2d/keyframe_program.h"
#include "models/model_2d/velocity_keyframe.h"

typedef struct BlendS {
    int   keyframe[2];
    float keyframeWeight[2];
    int   symbol[2];
    float symbolWeight[2];
} BlendT;

static BlendT blendAt(const KeyframeProgramT *program, float m, float theta)
{
    BlendT blend;

    float u = wrapf(theta, 0.0f, 2.0f*PI)/(2.0f*PI)*program->keyframes;
    int k0 = (int)floorf(u);
    float wk = u - k0;
    blend.keyframe[0] = wrapi(k0, program->keyframes);
    blend.keyframe[1] = wrapi(k0 + 1, program->keyframes);
    blend.keyframeWeight[0] = 1.0f - wk;
    blend.keyframeWeight[1] = wk;

    float v = 0.0f;
    if (program->symbols > 1)
    {
        v = (m - MEMORY_MIN_VALUE)/(MEMORY_MAX_VALUE - MEMORY_MIN_VALUE)*(program->symbols - 1);
        v = clamp(v, 0.0f, (float)(program->symbols - 1));
    }
    int s0 = (int)floorf(v);
    float ws = v - s0;
    blend.symbol[0] = s0;
    blend.symbol[1] = MIN(s0 + 1, program->symbols - 1);
    blend.symbolWeight[0] = 1.0f - ws;
    blend.symbolWeight[1] = ws;

    return blend;
}

static float complex evalComponent(const Model2DDataT *data, FieldComponentT component,
                                   float complex S, float m, float theta)
{
    const KeyframeProgramT *program = &data->program;
    BlendT blend = blendAt(program, m, theta);

    float complex result = 0.0f;
    for (int a = 0; a < 2; a++)
    {
        for (int b = 0; b < 2; b++)
        {
            float w = blend.keyframeWeight[a]*blend.symbolWeight[b];
            if (w == 0.0f)
            {
                continue;
            }
            int index = program->index[blend.keyframe[a]][blend.symbol[b]];
            const SplatListT *list = &program->functions[index].components[component];
            result += w*model2d_EvalSplats(list, S);
        }
    }
    return result;
}

static void init(Model2DDataT *data)
{
    model2d_InitKeyframeProgram(&data->program);
}

static float complex F_S(const Model2DDataT *data, float complex S, float m, float theta)
{
    return evalComponent(data, FIELD_F_S, S, m, theta);
}

static float F_X(const Model2DDataT *data, float complex S, float m, float theta)
{
    return crealf(evalComponent(data, FIELD_F_X, S, m, theta));
}

static float F_M(const Model2DDataT *data, float complex S, float m, float theta)
{
    return crealf(evalComponent(data, FIELD_F_M, S, m, theta));
}

const VelocityField2DT model2d_VelocityKeyframe = {
    .init = init,
    .F_S  = F_S,
    .F_X  = F_X,
    .F_M  = F_M,
};
