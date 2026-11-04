#ifndef __COMMON_H
#define __COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <raylib.h>

#define WIDTH 1020
#define HEIGHT 900

typedef struct ParametersS {
    float simBudget;
    int   maxSteps;
} ParametersT;

typedef struct SettingsS {
    ParametersT parameters;
    int selectedModel;
} SettingsT;

/*---------FUNCTIONS--------*/

static inline int wrapi(int i, int n)
{
    i %= n;
    if (i < 0) i += n;
    return i;
}

static inline float wrapf(float x, float min, float max)
{
    float L = max - min;

    while (x < min) x += L;
    while (x >= max) x -= L;

    return x;
}

static inline float clamp(float x, float min, float max)
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
