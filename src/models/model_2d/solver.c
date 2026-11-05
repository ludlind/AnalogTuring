#include <math.h>
#include <complex.h>
#include <stdbool.h>
#include <stdlib.h>

#include "common.h"
#include "lib/util.h"
#include "models/model_2d/solver.h"

float model2d_periodicMemoryDistance(float a, float b)
{
    float L = MEMORY_GRID_POINTS * MEMORY_GRID_SIZE;
    float d = a - b;

    d -= L * roundf(d / L);
    return d;
}

static float memoryWriteProfile(const Model2DDataT *data, float dx)
{
    return gaussian(dx, data->state.sigma_M);
}

static float complex clampToDisk(float complex S, float radius)
{
    float r = cabsf(S);
    if (r > radius)
    {
        return S * (radius / r);
    }
    return S;
}

float model2d_getMemoryStartTime(const Model2DDataT *data)
{
    return data->memory.currentMemTime - (TIME_MEMORY_GRID_POINTS-1)*data->state.dt;
}

static float getMemoryBufferStartTime(const Model2DDataT *data)
{
    return data->memory.currentMemTime - (data->memory.currentMemIndex-1)*data->state.dt;
}

float model2d_interpM(const Model2DDataT *data, float x)
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

static bool historyIndex(const Model2DDataT *data, float t, int *i1, int *i2, float *w)
{
    float tStart = model2d_getMemoryStartTime(data);
    if (t > data->memory.currentMemTime || t < tStart)
    {
        return false;
    }

    float tMemStart = getMemoryBufferStartTime(data);
    float tnorm = (t - tMemStart) / data->state.dt;
    int i = (int)floorf(tnorm);

    *w = tnorm - i;
    *i1 = wrapi(i, TIME_MEMORY_GRID_POINTS);
    *i2 = wrapi(i + 1, TIME_MEMORY_GRID_POINTS);
    return true;
}

static float interpHistory(const Model2DDataT *data, const float *history, float t)
{
    int i1, i2;
    float w;
    if (!historyIndex(data, t, &i1, &i2, &w))
    {
        return 0.0f;
    }
    return (1.0f - w)*history[i1] + w*history[i2];
}

float complex model2d_interpS(const Model2DDataT *data, float t)
{
    int i1, i2;
    float w;
    if (!historyIndex(data, t, &i1, &i2, &w))
    {
        return 0.0f;
    }
    return (1.0f - w)*data->memory.SMem[i1] + w*data->memory.SMem[i2];
}

float model2d_interpTheta(const Model2DDataT *data, float t)
{
    int i1, i2;
    float w;
    if (!historyIndex(data, t, &i1, &i2, &w))
    {
        return 0.0f;
    }
    float theta1 = data->memory.thetaMem[i1];
    float dtheta = wrapf(data->memory.thetaMem[i2] - theta1, -PI, PI);
    return wrapf(theta1 + w*dtheta, 0.0f, 2.0f*PI);
}

float model2d_interpX(const Model2DDataT *data, float t)
{
    return interpHistory(data, data->memory.XMem, t);
}

float model2d_interpm(const Model2DDataT *data, float t)
{
    return interpHistory(data, data->memory.mMem, t);
}

void model2d_allocState(Model2DDataT *data)
{
    data->state.M = (float *) malloc(MEMORY_GRID_POINTS * sizeof(float));
    data->state.Mnext = (float *) malloc(MEMORY_GRID_POINTS * sizeof(float));
}

void model2d_resetState(Model2DDataT *data)
{
    State2DT *state = &data->state;
    Memory2DT *memory = &data->memory;

    state->S = clampToDisk(data->startS, STATE_MAX_RADIUS);
    state->theta = 0.0f;
    state->X = 0.0f;
    state->m = 0.0f;
    state->sigma_M = data->parameters.sigma_M;
    state->omega = data->parameters.omega;
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
        memory->thetaMem[i] = 0.0f;
        memory->XMem[i] = 0.0f;
        memory->mMem[i] = 0.0f;
    }
}

void model2d_stepState(Model2DDataT *data)
{
    State2DT *state = &data->state;
    Memory2DT *memory = &data->memory;
    const VelocityField2DT *velocity = data->velocity;

    float complex S = state->S;
    float X = state->X;
    float m = state->m;
    float theta = state->theta;

    S += velocity->F_S(data, state->S, m, theta)*state->dt;
    X += velocity->F_X(data, state->S, m, theta)*state->dt;

    S = clampToDisk(S, STATE_MAX_RADIUS);
    X = wrapf(X, MEMORY_START_POSITION, MEMORY_STOP_POSITION);
    theta = wrapf(theta + state->omega*state->dt, 0.0f, 2.0f*PI);

    float A = velocity->F_M(data, state->S, m, state->theta);
    for (int i = 0; i < MEMORY_GRID_POINTS; i++)
    {
        float x = i*MEMORY_GRID_SIZE + MEMORY_GRID_OFFSET;
        float dx = model2d_periodicMemoryDistance(x, state->X);
        float M = state->M[i] + A * memoryWriteProfile(data, dx) * state->dt;
        state->Mnext[i] = clamp(M, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE);
    }

    float *Mprev = state->M;
    state->M = state->Mnext;
    state->Mnext = Mprev;
    state->S = S;
    state->X = X;
    state->theta = theta;
    state->m = model2d_interpM(data, X);
    state->t += state->dt;

    memory->SMem[memory->currentMemIndex] = state->S;
    memory->thetaMem[memory->currentMemIndex] = state->theta;
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

void model2d_freeState(Model2DDataT *data)
{
    free(data->state.M);
    free(data->state.Mnext);
    data->state.M = NULL;
    data->state.Mnext = NULL;
}
