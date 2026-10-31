#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "state.h"
#include "util.h"
#include "models/common.h"

int stateMap[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];

float FSvalues[NUMBER_OF_STATES];
float FXvalues[NUMBER_OF_STATES];
float FMvalues[NUMBER_OF_STATES];

float GetFSValue(int state)
{
    return FSvalues[state];
}

void SetFSValue(int state, float value)
{
    FSvalues[state] = value;
}

float GetFXValue(int state)
{
    return FXvalues[state];
}

void SetFXValue(int state, float value)
{
    FXvalues[state] = value;
}

float GetFMValue(int state)
{
    return FMvalues[state];
}

void SetFMValue(int state, float value)
{
    FMvalues[state] = value;
}

void StateMapSetValue(float S, float m, int state)
{

    int SIndex = clamp(floorf(S*NUMBER_OF_COLUMNS), 0, NUMBER_OF_COLUMNS-1);
    int mIndex = clamp(floorf(m*NUMBER_OF_ROWS), 0, NUMBER_OF_ROWS-1);

    stateMap[SIndex][mIndex] = state;
}

int StateMapGetValue(float S, float m)
{
    int SIndex = clamp(floorf(S*NUMBER_OF_COLUMNS), 0, NUMBER_OF_COLUMNS-1);
    int mIndex = clamp(floorf(m*NUMBER_OF_ROWS), 0, NUMBER_OF_ROWS-1);

    return stateMap[SIndex][mIndex];
}

float StateMapGetValueF(float S, float m)
{
    int SIndex = clamp(floorf(S*NUMBER_OF_COLUMNS), 0, NUMBER_OF_COLUMNS-1);
    int mIndex = clamp(floorf(m*NUMBER_OF_ROWS), 0, NUMBER_OF_ROWS-1);

    return (float)stateMap[SIndex][mIndex];
}



float* initM;

void initState(StateT* state_p)
{
    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        state_p->M[i] = initM[i];
    }
}

void destroyProgram(void)
{
    free(initM);
}
 
float interpInitM(float x)
{
    float min = getMemoryStartPosition();
    float max = getMemoryStopPosition();

    x = wrapf(x, min, max);
    float xnorm = (x - min) / MEMORY_GRID_SIZE;

    int i1 = (int)floorf(xnorm);
    int i2 = i1 + 1;
    float w = xnorm - i1;

    i1 = wrapi(i1, MEMORY_GRID_POINTS);
    i2 = wrapi(i2, MEMORY_GRID_POINTS);

    return (1.0f - w)*initM[i1] + w*initM[i2];
}

void writeGaussianInitM(float X, float intensity, float sigma)
{
    float Mmin = getMemoryMinValue();
    float Mmax = getMemoryMaxValue();
    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        float x = i*MEMORY_GRID_SIZE + MEMORY_GRID_OFFSET;
        float dx = periodicMemoryDistance(x, X);
        float M = initM[i] + intensity * gaussian(dx, sigma);
        initM[i] = clamp(M, Mmin, Mmax);
    }
}
