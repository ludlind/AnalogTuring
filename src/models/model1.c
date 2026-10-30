#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "state.h"
#include "util.h"
#include "models/common.h"
#include "models/model1.h"

void initModel1(void)
{
    initM = (float *) malloc(MEMORY_GRID_POINTS * sizeof(float));
    srand((unsigned int)time(NULL));
    for (int i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; j++)
        {
            stateMap[i][j] = rand() % NUMBER_OF_STATES;
        }
    }
    for (int k = 0; k < NUMBER_OF_STATES; k++)
    {
        FSvalues[k] = ((float)rand()/(float)(RAND_MAX)) * 0.1f - 0.05f;
        FXvalues[k] = ((float)rand()/(float)(RAND_MAX)) * 0.1f - 0.05f;
        FMvalues[k] = ((float)rand()/(float)(RAND_MAX)) * 0.1f - 0.05f;
    }
}

float gaussian_states_F_S(float S, float m)
{
    float SMin = getStateMinValue();
    float SMax = getStateMaxValue();
    float mMin = getMemoryMinValue();
    float mMax = getMemoryMaxValue();

    float dS = (SMax - SMin)/(NUMBER_OF_COLUMNS-1);
    float dm = (mMax - mMin)/(NUMBER_OF_ROWS-1);

    int SIndexMin = MAX(ceilf((S - SMin - 3*SIGMA_FS) / dS), 0);
    int SIndexMax = MIN(floorf((S - SMin + 3*SIGMA_FS) / dS), NUMBER_OF_COLUMNS-1);
    int mIndexMin = MAX(ceilf((m - mMin - 3*SIGMA_FS) / dm), 0);
    int mIndexMax = MIN(floorf((m - mMin + 3*SIGMA_FS) / dm), NUMBER_OF_ROWS-1);

    float result = 0.0f;
    for (int i = SIndexMin; i <= SIndexMax; i++)
    {
        for (int j = mIndexMin; j <= mIndexMax; j++)
        {
            float diffS = (dS*i - (S - SMin))/(SMax - SMin);
            float diffm = (dm*j - (m - mMin))/(mMax - mMin);
            float distance = sqrt(diffS*diffS + diffm*diffm);
            if (distance > 3*SIGMA_FS)
            {
                continue;
            }

            float scaleCoeff = FSvalues[stateMap[i][j]];
            result += scaleCoeff * gaussian(distance, SIGMA_FS);
        }
    }
    return result;
}

float gaussian_states_F_X(float S, float m)
{
    float SMin = getStateMinValue();
    float SMax = getStateMaxValue();
    float mMin = getMemoryMinValue();
    float mMax = getMemoryMaxValue();

    float dS = (SMax - SMin)/(NUMBER_OF_COLUMNS-1);
    float dm = (mMax - mMin)/(NUMBER_OF_ROWS-1);

    int SIndexMin = MAX(ceilf((S - SMin - 3*SIGMA_FX) / dS), 0);
    int SIndexMax = MIN(floorf((S - SMin + 3*SIGMA_FX) / dS), NUMBER_OF_COLUMNS-1);
    int mIndexMin = MAX(ceilf((m - mMin - 3*SIGMA_FX) / dm), 0);
    int mIndexMax = MIN(floorf((m - mMin + 3*SIGMA_FX) / dm), NUMBER_OF_ROWS-1);

    float result = 0.0f;
    for (int i = SIndexMin; i <= SIndexMax; i++)
    {
        for (int j = mIndexMin; j <= mIndexMax; j++)
        {
            float diffS = (dS*i - (S - SMin))/(SMax - SMin);
            float diffm = (dm*j - (m - mMin))/(mMax - mMin);
            float distance = sqrt(diffS*diffS + diffm*diffm);
            if (distance > 3*SIGMA_FX)
            {
                continue;
            }

            float scaleCoeff = FXvalues[stateMap[i][j]];
            result += scaleCoeff * gaussian(distance, SIGMA_FX);
        }
    }
    return result;
}

float gaussian_states_F_M(float S, float m)
{
    float SMin = getStateMinValue();
    float SMax = getStateMaxValue();
    float mMin = getMemoryMinValue();
    float mMax = getMemoryMaxValue();

    float dS = (SMax - SMin)/(NUMBER_OF_COLUMNS-1);
    float dm = (mMax - mMin)/(NUMBER_OF_ROWS-1);

    int SIndexMin = MAX(ceilf((S - SMin - 3*SIGMA_FM) / dS), 0);
    int SIndexMax = MIN(floorf((S - SMin + 3*SIGMA_FM) / dS), NUMBER_OF_COLUMNS-1);
    int mIndexMin = MAX(ceilf((m - mMin - 3*SIGMA_FM) / dm), 0);
    int mIndexMax = MIN(floorf((m - mMin + 3*SIGMA_FM) / dm), NUMBER_OF_ROWS-1);

    float result = 0.0f;
    for (int i = SIndexMin; i <= SIndexMax; i++)
    {
        for (int j = mIndexMin; j <= mIndexMax; j++)
        {
            float diffS = (dS*i - (S - SMin))/(SMax - SMin);
            float diffm = (dm*j - (m - mMin))/(mMax - mMin);
            float distance = sqrt(diffS*diffS + diffm*diffm);
            if (distance > 3*SIGMA_FM)
            {
                continue;
            }

            int stateIndex = stateMap[i][j];
            assert(stateIndex >= 0 && stateIndex < NUMBER_OF_STATES);
            float scaleCoeff = FMvalues[stateIndex];
            result += scaleCoeff * gaussian(distance, SIGMA_FM);
        }
    }
    return result;
}

float gaussian_states_dMdt(StateT *state_p, float dx)
{
    float S = state_p->S;
    float m = state_p->m;
    float A = gaussian_states_F_M(S, m);

    float sigma_M = state_p->sigma_M;
    return A * gaussian(dx, sigma_M);
}
