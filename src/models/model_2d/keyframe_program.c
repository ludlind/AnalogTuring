#include <math.h>
#include <complex.h>
#include <string.h>

#include "lib/util.h"
#include "models/model_2d/keyframe_program.h"

#define DEFAULT_KEYFRAMES 4
#define DEFAULT_SYMBOLS   2

void model2d_InitKeyframeProgram(KeyframeProgramT *program)
{
    memset(program, 0, sizeof(*program));
    program->keyframes = DEFAULT_KEYFRAMES;
    program->symbols = DEFAULT_SYMBOLS;
}

float model2d_KeyframeTheta(const KeyframeProgramT *program, int keyframe)
{
    return 2.0f*PI*keyframe/program->keyframes;
}

float model2d_SymbolLevel(const KeyframeProgramT *program, int symbol)
{
    if (program->symbols < 2)
    {
        return 0.5f*(MEMORY_MIN_VALUE + MEMORY_MAX_VALUE);
    }
    return MEMORY_MIN_VALUE + (MEMORY_MAX_VALUE - MEMORY_MIN_VALUE)*symbol/(program->symbols - 1);
}

int model2d_AddSplat(SplatListT *list, SplatT splat)
{
    if (list->count >= MAX_SPLATS)
    {
        return -1;
    }
    list->splats[list->count] = splat;
    return list->count++;
}

void model2d_RemoveSplat(SplatListT *list, int k)
{
    if (k < 0 || k >= list->count)
    {
        return;
    }
    for (int i = k; i < list->count - 1; i++)
    {
        list->splats[i] = list->splats[i + 1];
    }
    list->count--;
}

float complex model2d_EvalSplats(const SplatListT *list, float complex S)
{
    float complex result = 0.0f;
    for (int k = 0; k < list->count; k++)
    {
        const SplatT *splat = &list->splats[k];
        float complex d = S - splat->center;
        float r2 = crealf(d)*crealf(d) + cimagf(d)*cimagf(d);
        float g = expf(-r2/(2.0f*splat->sigma*splat->sigma));
        result += (splat->value + splat->alpha*d + splat->beta*conjf(d))*g;
    }
    return result;
}
