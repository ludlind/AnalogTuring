#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "common.h"
#include "lib/util.h"
#include "models/model_1d/solver.h"

float periodicMemoryDistance(float a, float b)
{
    float L = MEMORY_GRID_POINTS * MEMORY_GRID_SIZE;
    float d = a - b;

    d -= L * roundf(d / L);
    return d;
}

static float memoryWriteProfile(const Model1DDataT *data, float dx)
{
    return gaussian(dx, data->state.sigma_M);
}

float getMemoryStartTime(const Model1DDataT *data)
{
    return data->memory.currentMemTime - (TIME_MEMORY_GRID_POINTS-1)*data->state.dt;
}

static float getMemoryBufferStartTime(const Model1DDataT *data)
{
    return data->memory.currentMemTime - (data->memory.currentMemIndex-1)*data->state.dt;
}

float interpM(const Model1DDataT *data, float x)
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

    return (1.0f - w)*data->state.M[i1] + w*data->state.M[i2];
}

static float interpHistory(const Model1DDataT *data, const float *history, float t)
{
    float tStart = getMemoryStartTime(data);
    if (t > data->memory.currentMemTime || t < tStart)
    {
        return 0.0f;
    }

    float tMemStart = getMemoryBufferStartTime(data);
    float tnorm = (t - tMemStart) / data->state.dt;
    int i1 = floorf(tnorm);
    int i2 = i1 + 1;
    float w = tnorm - i1;

    i1 = wrapi(i1, TIME_MEMORY_GRID_POINTS);
    i2 = wrapi(i2, TIME_MEMORY_GRID_POINTS);

    return (1.0f - w)*history[i1] + w*history[i2];
}

float interpS(const Model1DDataT *data, float t)
{
    return interpHistory(data, data->memory.SMem, t);
}

float interpX(const Model1DDataT *data, float t)
{
    return interpHistory(data, data->memory.XMem, t);
}

float interpm(const Model1DDataT *data, float t)
{
    return interpHistory(data, data->memory.mMem, t);
}

void allocState(Model1DDataT *data)
{
    data->state.M = (float *) malloc(MEMORY_GRID_POINTS * sizeof(float));
    data->state.Mnext = (float *) malloc(MEMORY_GRID_POINTS * sizeof(float));
}

void resetState(Model1DDataT *data)
{
    StateT *state = &data->state;
    MemoryT *memory = &data->memory;

    state->S = data->startS;
    state->X = 0.0f;
    state->m = 0.0f;
    state->sigma_M = data->parameters.sigma_M;
    state->dt = data->parameters.dt;
    state->t = 0.0f;

    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        state->M[i] = data->initM[i];
    }

    memory->currentMemIndex = 0;
    memory->currentMemTime = 0.0f;
    memory->wrapMem = false;

    for (int i = 0; i < TIME_MEMORY_GRID_POINTS; i++)
    {
        memory->SMem[i] = 0.0f;
        memory->XMem[i] = 0.0f;
        memory->mMem[i] = 0.0f;
    }
}

void stepState(Model1DDataT *data)
{
    StateT *state = &data->state;
    MemoryT *memory = &data->memory;
    const VelocityFieldT *velocity = data->velocity;

    // CALCULATE NEW STATE

    float S = state->S;
    float X = state->X;

    S += velocity->F_S(data, state->S, state->m)*state->dt;
    X += velocity->F_X(data, state->S, state->m)*state->dt;

    S = clamp(S, STATE_MIN_VALUE, STATE_MAX_VALUE);
    X = wrapf(X, MEMORY_START_POSITION, MEMORY_STOP_POSITION);

    float A = velocity->F_M(data, state->S, state->m);
    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        float x = i*MEMORY_GRID_SIZE + MEMORY_GRID_OFFSET;
        float dx = periodicMemoryDistance(x, state->X);
        float M = state->M[i] + A * memoryWriteProfile(data, dx) * state->dt;
        state->Mnext[i] = clamp(M, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE);
    }

    // UPDATE STATE

    float *Mprev = state->M;
    state->M = state->Mnext;
    state->Mnext = Mprev;
    state->S = S;
    state->X = X;
    state->m = interpM(data, X);
    state->t += state->dt;

    // STATE MEMORY

    memory->SMem[memory->currentMemIndex] = state->S;
    memory->XMem[memory->currentMemIndex] = state->X;
    memory->mMem[memory->currentMemIndex] = state->m;

    memory->currentMemIndex++;
    if (memory->currentMemIndex >= TIME_MEMORY_GRID_POINTS)
    {
        memory->currentMemIndex = 0;
        memory->wrapMem = true;
    }
    memory->currentMemTime = state->t;
}

void freeState(Model1DDataT *data)
{
    free(data->state.M);
    free(data->state.Mnext);
    data->state.M = NULL;
    data->state.Mnext = NULL;
}
