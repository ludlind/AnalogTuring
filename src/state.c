#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "common.h"
#include "state.h"
#include "program.h"

static StateT state;
static MemoryT memory;

float periodicMemoryDistance(float a, float b)
{
    float L = MEMORY_GRID_POINTS * MEMORY_GRID_SIZE;
    float d = a - b;

    d -= L * roundf(d / L);
    return d;
}

float getS(void)
{
    return state.S;
}

float getX(void)
{
    return state.X;
}

float getm(void)
{
    return state.m;
}

float getTime(void)
{
    return state.t;
}

float getDT(void)
{
    return state.dt;
}

float getMemoryStartTime(void)
{
    return memory.currentMemTime - (TIME_MEMORY_GRID_POINTS-1)*state.dt;
}

float getMemoryBufferStartTime(void)
{
    return memory.currentMemTime - (memory.currentMemIndex-1)*state.dt;
}

float getMemoryStartPosition(void)
{
   return MEMORY_GRID_OFFSET; 
}

float getMemoryStopPosition(void)
{
   return MEMORY_GRID_OFFSET + (MEMORY_GRID_POINTS-1) * MEMORY_GRID_SIZE; 
}

float getMemoryMinValue(void)
{
   return MEMORY_MIN_VALUE; 
}

float getMemoryMaxValue(void)
{
   return MEMORY_MAX_VALUE; 
}

float getStateMinValue(void)
{
   return STATE_MIN_VALUE; 
}

float getStateMaxValue(void)
{
   return STATE_MAX_VALUE; 
}

float interpM(float x)
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

    return (1.0f - w)*state.M[i1] + w*state.M[i2];
}

float interpS(float t)
{
    float tStart = getMemoryStartTime();
    if (t > memory.currentMemTime || t < tStart)
    {
        return 0.0f;
    }

    float tMemStart = getMemoryBufferStartTime();
    float tnorm = (t - tMemStart) / state.dt;
    int i1 = floorf(tnorm);
    int i2 = i1 + 1;
    float w = tnorm - i1;

    i1 = wrapi(i1, TIME_MEMORY_GRID_POINTS);
    i2 = wrapi(i2, TIME_MEMORY_GRID_POINTS);

    return (1.0f - w)*memory.SMem[i1] + w*memory.SMem[i2];
}

float interpX(float t)
{
    float tStart = getMemoryStartTime();
    if (t > memory.currentMemTime || t < tStart)
    {
        return 0.0f;
    }

    float tMemStart = getMemoryBufferStartTime();
    float tnorm = (t - tMemStart) / state.dt;
    int i1 = floorf(tnorm);
    int i2 = i1 + 1;
    float w = tnorm - i1;

    i1 = wrapi(i1, TIME_MEMORY_GRID_POINTS);
    i2 = wrapi(i2, TIME_MEMORY_GRID_POINTS);

    return (1.0f - w)*memory.XMem[i1] + w*memory.XMem[i2];
}

float interpm(float t)
{
    float tStart = getMemoryStartTime();
    if (t > memory.currentMemTime || t < tStart)
    {
        return 0.0f;
    }

    float tMemStart = getMemoryBufferStartTime();
    float tnorm = (t - tMemStart) / state.dt;
    int i1 = floorf(tnorm);
    int i2 = i1 + 1;
    float w = tnorm - i1;

    i1 = wrapi(i1, TIME_MEMORY_GRID_POINTS);
    i2 = wrapi(i2, TIME_MEMORY_GRID_POINTS);

    return (1.0f - w)*memory.mMem[i1] + w*memory.mMem[i2];
}

void InitState(ParametersT* parameters_p, ProgramT* program_p)
{
    state.S = program_p->startS;
    state.X = 0.0f;
    state.m = 0.0f;
    state.sigma_M = parameters_p->sigma_M;
    state.dt = parameters_p->dt;
    state.t = 0.0f;

    free(state.M);
    free(state.Mnext);
    state.M = (float *) malloc(MEMORY_GRID_POINTS * sizeof(float));
    state.Mnext = (float *) malloc(MEMORY_GRID_POINTS * sizeof(float));

    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        state.M[i] = 0.0f;
    }
    
    memory.currentMemIndex = 0;
    memory.currentMemTime = 0.0f;
    memory.wrapMem = false;

    for (int i = 0; i < TIME_MEMORY_GRID_POINTS; i++)
    {
        memory.SMem[i] = 0.0f;
        memory.XMem[i] = 0.0f;
        memory.mMem[i] = 0.0f;
    }

    program_p->InitState(&state);
}

void UpdateState(ProgramT *program_p)
{
    // CALCULATE NEW STATE

    float Smin = getStateMinValue();
    float Smax = getStateMaxValue();
    
    float Xmin = getMemoryStartPosition();
    float Xmax = getMemoryStopPosition();

    float Mmin = getMemoryMinValue();
    float Mmax = getMemoryMaxValue();
    
    float S = state.S;
    float X = state.X;

    S += program_p->F_S(state.S, state.m)*state.dt;
    X += program_p->F_X(state.S, state.m)*state.dt;

    S = clamp(S, Smin, Smax);
    X = wrapf(X, Xmin, Xmax);

    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        float x = i*MEMORY_GRID_SIZE + MEMORY_GRID_OFFSET;
        float dx = periodicMemoryDistance(x, state.X);
        float M = state.M[i] + program_p->dMdt(&state, dx) * state.dt;
        state.Mnext[i] = clamp(M, Mmin, Mmax);
    }

    // UPDATE STATE
   
    float *Mprev = state.M; 
    state.M = state.Mnext;
    state.Mnext = Mprev;
    state.S = S;
    state.X = X;
    state.m = interpM(X);
    state.t += state.dt;

    // STATE MEMORY

    memory.SMem[memory.currentMemIndex] = state.S;
    memory.XMem[memory.currentMemIndex] = state.X;
    memory.mMem[memory.currentMemIndex] = interpM(state.X);

    memory.currentMemIndex++;
    if (memory.currentMemIndex >= TIME_MEMORY_GRID_POINTS)
    {
        memory.currentMemIndex = 0;
        memory.wrapMem = true;
    }
    memory.currentMemTime = state.t;
}

void DestroyState(void)
{
    free(state.M);
    free(state.Mnext);
}
