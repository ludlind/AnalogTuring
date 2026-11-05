#include <math.h>

#include "lib/util.h"
#include "models/model_1d/velocity_pixel.h"

static void init(Model1DDataT *data)
{
    for (int i = 0; i < NUMBER_OF_COLUMNS; i++)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; j++)
        {
            data->stateMap[i][j] = 0;
        }
    }
    for (int k = 0; k < NUMBER_OF_STATES; k++)
    {
        data->FS[k] = 0;
        data->FX[k] = 0;
        data->FM[k] = 0;
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

    int SIndex = MIN(MAX(roundf((S - SMin) / dS), 0), NUMBER_OF_COLUMNS-1);
    int mIndex = MIN(MAX(roundf((m - mMin) / dm), 0), NUMBER_OF_ROWS-1);

    return data->FS[data->stateMap[SIndex][mIndex]];
}

static float F_X(const Model1DDataT *data, float S, float m)
{
    float SMin = STATE_MIN_VALUE;
    float SMax = STATE_MAX_VALUE;
    float mMin = MEMORY_MIN_VALUE;
    float mMax = MEMORY_MAX_VALUE;

    float dS = (SMax - SMin)/(NUMBER_OF_COLUMNS-1);
    float dm = (mMax - mMin)/(NUMBER_OF_ROWS-1);

    int SIndex = MIN(MAX(roundf((S - SMin) / dS), 0), NUMBER_OF_COLUMNS-1);
    int mIndex = MIN(MAX(roundf((m - mMin) / dm), 0), NUMBER_OF_ROWS-1);

    return data->FX[data->stateMap[SIndex][mIndex]];
}

static float F_M(const Model1DDataT *data, float S, float m)
{
    float SMin = STATE_MIN_VALUE;
    float SMax = STATE_MAX_VALUE;
    float mMin = MEMORY_MIN_VALUE;
    float mMax = MEMORY_MAX_VALUE;

    float dS = (SMax - SMin)/(NUMBER_OF_COLUMNS-1);
    float dm = (mMax - mMin)/(NUMBER_OF_ROWS-1);

    int SIndex = MIN(MAX(roundf((S - SMin) / dS), 0), NUMBER_OF_COLUMNS-1);
    int mIndex = MIN(MAX(roundf((m - mMin) / dm), 0), NUMBER_OF_ROWS-1);

    return data->FM[data->stateMap[SIndex][mIndex]];
}

const VelocityFieldT model1d_VelocityPixel = {
    .init = init,
    .F_S  = F_S,
    .F_X  = F_X,
    .F_M  = F_M,
};
