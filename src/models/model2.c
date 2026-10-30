#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "state.h"
#include "util.h"
#include "models/common.h"
#include "models/model2.h"

void initModel2(void)
{
    initM = (float *) malloc(MEMORY_GRID_POINTS * sizeof(float));
    //srand((unsigned int)time(NULL));
    for (int i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; j++)
        {
            stateMap[i][j] = 0;//rand() % NUMBER_OF_STATES;
        }
    }
    for (int k = 0; k < NUMBER_OF_STATES; k++)
    {
        FSvalues[k] = 0;//0.5f*((rand() % 5)*0.25f - 0.5f);
        FXvalues[k] = 0;//2.0f*((rand() % 5)*0.25f - 0.5f);
        FMvalues[k] = 0;//0.5f*((rand() % 5)*0.25f - 0.5f);
    }
}

float pixel_F_S(float S, float m)
{
    float SMin = getStateMinValue();
    float SMax = getStateMaxValue();
    float mMin = getMemoryMinValue();
    float mMax = getMemoryMaxValue();

    float dS = (SMax - SMin)/(NUMBER_OF_COLUMNS-1);
    float dm = (mMax - mMin)/(NUMBER_OF_ROWS-1);

    int SIndex = MIN(MAX(roundf((S - SMin) / dS), 0), NUMBER_OF_COLUMNS-1);
    int mIndex = MIN(MAX(roundf((m - mMin) / dm), 0), NUMBER_OF_ROWS-1);

    return FSvalues[stateMap[SIndex][mIndex]];
}

float pixel_F_X(float S, float m)
{
    float SMin = getStateMinValue();
    float SMax = getStateMaxValue();
    float mMin = getMemoryMinValue();
    float mMax = getMemoryMaxValue();

    float dS = (SMax - SMin)/(NUMBER_OF_COLUMNS-1);
    float dm = (mMax - mMin)/(NUMBER_OF_ROWS-1);

    int SIndex = MIN(MAX(roundf((S - SMin) / dS), 0), NUMBER_OF_COLUMNS-1);
    int mIndex = MIN(MAX(roundf((m - mMin) / dm), 0), NUMBER_OF_ROWS-1);

    return FXvalues[stateMap[SIndex][mIndex]];
}

float pixel_F_M(float S, float m)
{
    float SMin = getStateMinValue();
    float SMax = getStateMaxValue();
    float mMin = getMemoryMinValue();
    float mMax = getMemoryMaxValue();

    float dS = (SMax - SMin)/(NUMBER_OF_COLUMNS-1);
    float dm = (mMax - mMin)/(NUMBER_OF_ROWS-1);

    int SIndex = MIN(MAX(roundf((S - SMin) / dS), 0), NUMBER_OF_COLUMNS-1);
    int mIndex = MIN(MAX(roundf((m - mMin) / dm), 0), NUMBER_OF_ROWS-1);

    return FMvalues[stateMap[SIndex][mIndex]];
}

float pixel_dMdt(StateT *state_p, float dx)
{
    float S = state_p->S;
    float m = state_p->m;
    float A = pixel_F_M(S, m);

    float sigma_M = state_p->sigma_M;
    return A * gaussian(dx, sigma_M);
}
