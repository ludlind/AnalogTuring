#ifndef __COMMON_H
#define __COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <raylib.h>

#define DT 0.1f

#define MEMORY_GRID_POINTS 200001
#define MEMORY_GRID_SIZE 0.00001f
#define MEMORY_GRID_OFFSET -1.0f

#define MEMORY_MIN_VALUE -1.0f
#define MEMORY_MAX_VALUE 1.0f

#define STATE_MIN_VALUE -1.0f
#define STATE_MAX_VALUE 1.0f

#define TIME_MEMORY_GRID_POINTS 10001

#define WIDTH 980
#define HEIGHT 900

typedef struct AppS {
    MouseCursor cursor;
} AppT;

typedef struct StateS {
    float S;
    float X;
    float *M;
    float *Mnext;
    float m;
    float sigma_M;
    float dt;
    float t;
} StateT;

typedef struct MemoryS {
    int   currentMemIndex;
    float currentMemTime;
    bool  wrapMem;
    float SMem[TIME_MEMORY_GRID_POINTS];
    float XMem[TIME_MEMORY_GRID_POINTS];
    float mMem[TIME_MEMORY_GRID_POINTS];
} MemoryT;

typedef struct ProgramS {
    void  (*Init)(void);
    void  (*Destroy)(void);
    void  (*InitState)(StateT*);
    float (*F_S)(float, float);
    float (*F_X)(float, float);
    float (*F_M)(float, float);
    float (*dMdt)(StateT*, float);
    float startS;
} ProgramT;

typedef struct ParametersS {
    float simBudget;
    int maxSteps;
    float dt;
    float sigma_M;
} ParametersT;

/*---------FUNCTIONS--------*/

static int wrapi(int i, int n)
{
    i %= n;
    if (i < 0) i += n;
    return i;
}

static float wrapf(float x, float min, float max)
{
    float L = max - min;

    while (x < min) x += L;
    while (x >= max) x -= L;

    return x;
}

static float clamp(float x, float min, float max)
{
    if (x < min)
    {
        return min;
    }
    else if (x > max)
    {
        return max;
    }
    return x;
}

#endif
