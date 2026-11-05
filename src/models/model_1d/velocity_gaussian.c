#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "lib/util.h"
#include "models/model_1d/velocity_gaussian.h"

#define SIGMA_FS 0.1f
#define SIGMA_FX 0.1f
#define SIGMA_FM 0.1f

static void init(Model1DDataT *data)
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; j++)
        {
            data->stateMap[i][j] = rand() % NUMBER_OF_STATES;
        }
    }
    for (int k = 0; k < NUMBER_OF_STATES; k++)
    {
        data->FS[k] = ((float)rand()/(float)(RAND_MAX)) * 0.1f - 0.05f;
        data->FX[k] = ((float)rand()/(float)(RAND_MAX)) * 0.1f - 0.05f;
        data->FM[k] = ((float)rand()/(float)(RAND_MAX)) * 0.1f - 0.05f;
    }
}

static float F_S(const Model1DDataT *data, float S, float m)
{
    float SMin = STATE_MIN_VALUE;
    float SMax = STATE_MAX_VALUE;
    float mMin = MEMORY_MIN_VALUE;
    float mMax = MEMORY_MAX_VALUE;

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

            float scaleCoeff = data->FS[data->stateMap[i][j]];
            result += scaleCoeff * gaussian(distance, SIGMA_FS);
        }
    }
    return result;
}

static float F_X(const Model1DDataT *data, float S, float m)
{
    float SMin = STATE_MIN_VALUE;
    float SMax = STATE_MAX_VALUE;
    float mMin = MEMORY_MIN_VALUE;
    float mMax = MEMORY_MAX_VALUE;

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

            float scaleCoeff = data->FX[data->stateMap[i][j]];
            result += scaleCoeff * gaussian(distance, SIGMA_FX);
        }
    }
    return result;
}

static float F_M(const Model1DDataT *data, float S, float m)
{
    float SMin = STATE_MIN_VALUE;
    float SMax = STATE_MAX_VALUE;
    float mMin = MEMORY_MIN_VALUE;
    float mMax = MEMORY_MAX_VALUE;

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

            int stateIndex = data->stateMap[i][j];
            assert(stateIndex >= 0 && stateIndex < NUMBER_OF_STATES);
            float scaleCoeff = data->FM[stateIndex];
            result += scaleCoeff * gaussian(distance, SIGMA_FM);
        }
    }
    return result;
}

const VelocityFieldT model1d_VelocityGaussian = {
    .init = init,
    .F_S  = F_S,
    .F_X  = F_X,
    .F_M  = F_M,
};
