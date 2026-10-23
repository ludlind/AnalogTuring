#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "state.h"
#include "program.h"

#define MIN(a, b) (a > b ? b : a)
#define MAX(a, b) (a < b ? b : a)

#define USE_MODEL 2

static float gaussian(float dx, float sigma)
{
    float N = 1.0f/sqrt(2.0f*PI)/sigma;
    float a = dx/sigma;
    return N * expf(-a*a/2.0f);
}

//MODEL 0

static void initModel0(void)
{
}

// dSdt
static float F_S(float S, float m)
{
    float a = sinf(2*S/PI);
    return -a;
}

// dXdt
static float F_X(float S, float m)
{
    float r = sqrtf(S*S + m*m);
    float a = sinf(2*r/PI);
    return 1.0f;
}

static float F_M(float S, float m)
{
    return 0.1f;
}

static float dMdt(StateT *state_p, float dx)
{
    float S = state_p->S;
    float m = state_p->m;
    float A = F_M(S, m);

    float sigma_M = state_p->sigma_M;
    return A * gaussian(dx, sigma_M);
}

// MODEL 1/2

static int stateMap[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];

static float FSvalues[NUMBER_OF_STATES];
static float FXvalues[NUMBER_OF_STATES];
static float FMvalues[NUMBER_OF_STATES];

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

static ProgramT programs[];

float GetStartS(void)
{
    return programs[USE_MODEL].startS;
}

void SetStartS(float S)
{
    programs[USE_MODEL].startS = S;
}

static float* initM;

static void initState(StateT* state_p)
{
    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        state_p->M[i] = initM[i];
    }
}

static void destroyProgram(void)
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

float writeGaussianInitM(float X, float intensity, float sigma)
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


// MODEL 1

static void initModel1(void)
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

static float gaussian_states_F_S(float S, float m)
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

static float gaussian_states_F_X(float S, float m)
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

static float gaussian_states_F_M(float S, float m)
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

static float gaussian_states_dMdt(StateT *state_p, float dx)
{
    float S = state_p->S;
    float m = state_p->m;
    float A = gaussian_states_F_M(S, m);

    float sigma_M = state_p->sigma_M;
    return A * gaussian(dx, sigma_M);
}

// MODEL 2

static void initModel2(void)
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

static float pixel_F_S(float S, float m)
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

static float pixel_F_X(float S, float m)
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

static float pixel_F_M(float S, float m)
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

static float pixel_dMdt(StateT *state_p, float dx)
{
    float S = state_p->S;
    float m = state_p->m;
    float A = pixel_F_M(S, m);

    float sigma_M = state_p->sigma_M;
    return A * gaussian(dx, sigma_M);
}

// PROGRAM LIST

static ProgramT programs[] = {
    {initModel0, destroyProgram, initState,
     F_S, F_X, F_M, dMdt, 0.0f},
    {initModel1, destroyProgram, initState,
     gaussian_states_F_S, gaussian_states_F_X,
     gaussian_states_F_M, gaussian_states_dMdt, 0.0f},
    {initModel2, destroyProgram, initState,
     pixel_F_S, pixel_F_X, pixel_F_M, pixel_dMdt, 0.0f},
};

ProgramT *getProgram(void)
{
    static bool initiated = false;
    if (!initiated)
    {
        programs[USE_MODEL].Init();
        initiated = true;
    }

    return &programs[USE_MODEL];
}
