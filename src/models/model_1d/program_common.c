#include <math.h>

#include "lib/util.h"
#include "models/model_1d/solver.h"
#include "models/model_1d/program_common.h"

void StateMapSetValue(Model1DDataT *data, float S, float m, int state)
{
    int SIndex = clamp(floorf(S*NUMBER_OF_COLUMNS), 0, NUMBER_OF_COLUMNS-1);
    int mIndex = clamp(floorf(m*NUMBER_OF_ROWS), 0, NUMBER_OF_ROWS-1);

    data->stateMap[SIndex][mIndex] = state;
}

int StateMapGetValue(const Model1DDataT *data, float S, float m)
{
    int SIndex = clamp(floorf(S*NUMBER_OF_COLUMNS), 0, NUMBER_OF_COLUMNS-1);
    int mIndex = clamp(floorf(m*NUMBER_OF_ROWS), 0, NUMBER_OF_ROWS-1);

    return data->stateMap[SIndex][mIndex];
}

float StateMapGetValueF(const Model1DDataT *data, float S, float m)
{
    int SIndex = clamp(floorf(S*NUMBER_OF_COLUMNS), 0, NUMBER_OF_COLUMNS-1);
    int mIndex = clamp(floorf(m*NUMBER_OF_ROWS), 0, NUMBER_OF_ROWS-1);

    return (float)data->stateMap[SIndex][mIndex];
}

float interpInitM(const Model1DDataT *data, float x)
{
    float min = MEMORY_START_POSITION;
    float max = MEMORY_STOP_POSITION;

    x = wrapf(x, min, max);
    float xnorm = (x - min) / MEMORY_GRID_SIZE;

    int i1 = (int)floorf(xnorm);
    int i2 = i1 + 1;
    float w = xnorm - i1;

    i1 = wrapi(i1, MEMORY_GRID_POINTS);
    i2 = wrapi(i2, MEMORY_GRID_POINTS);

    return (1.0f - w)*data->initM[i1] + w*data->initM[i2];
}

void writeGaussianInitM(Model1DDataT *data, float X, float intensity, float sigma)
{
    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        float x = i*MEMORY_GRID_SIZE + MEMORY_GRID_OFFSET;
        float dx = periodicMemoryDistance(x, X);
        float M = data->initM[i] + intensity * gaussian(dx, sigma);
        data->initM[i] = clamp(M, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE);
    }
}
