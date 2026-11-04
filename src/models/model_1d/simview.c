#include <raylib.h>

#include "gui/visual.h"
#include "models/model_1d/solver.h"
#include "models/model_1d/simview.h"

static Rectangle FSMapGr = { 20, 20, 300, 300 };
static Rectangle FMMapGr = { 340, 20, 300, 300 };
static Rectangle FXMapGr = { 660, 20, 300, 300 };
static Rectangle SMemGr  = { 20, 340, 300, 200 };
static Rectangle XMemGr  = { 340, 340, 300, 200 };
static Rectangle mMemGr  = { 660, 340, 300, 200 };

static float plotFS(const void *ctx, float S, float m)
{
    const Model1DDataT *data = ctx;
    return data->velocity->F_S(data, S, m);
}

static float plotFX(const void *ctx, float S, float m)
{
    const Model1DDataT *data = ctx;
    return data->velocity->F_X(data, S, m);
}

static float plotFM(const void *ctx, float S, float m)
{
    const Model1DDataT *data = ctx;
    return data->velocity->F_M(data, S, m);
}

static float plotSHistory(const void *ctx, float t)
{
    return interpS(ctx, t);
}

static float plotXHistory(const void *ctx, float t)
{
    return interpX(ctx, t);
}

static float plotMemory(const void *ctx, float x)
{
    return interpM(ctx, x);
}

void DrawSimulationView(void *data)
{
    Model1DDataT *d = model1d_of(data);

    float S = d->state.S;
    float m = d->state.m;

    float t1 = getMemoryStartTime(d);
    float t2 = d->state.t;

    float Smin = STATE_MIN_VALUE;
    float Smax = STATE_MAX_VALUE;

    float Xmin = MEMORY_START_POSITION;
    float Xmax = MEMORY_STOP_POSITION;

    float Mmin = MEMORY_MIN_VALUE;
    float Mmax = MEMORY_MAX_VALUE;

    DrawMap(FSMapGr, 5, 2, 0, plotFS, d, Smin, Smax, Mmin, Mmax, -1.0f, 1.0f);
    DrawMap(FMMapGr, 5, 2, 0, plotFM, d, Smin, Smax, Mmin, Mmax, -1.0f, 1.0f);
    DrawMap(FXMapGr, 5, 2, 0, plotFX, d, Smin, Smax, Mmin, Mmax, -1.0f, 1.0f);

    DrawDot(FSMapGr, 2, S, m, Smin, Smax, Mmin, Mmax, RAYWHITE);
    DrawDot(FMMapGr, 2, S, m, Smin, Smax, Mmin, Mmax, RAYWHITE);
    DrawDot(FXMapGr, 2, S, m, Smin, Smax, Mmin, Mmax, RAYWHITE);

    DrawScope(SMemGr, 2, plotSHistory, d, t1, t2, Smin, Smax);
    DrawScope(XMemGr, 2, plotXHistory, d, t1, t2, Xmin, Xmax);
    DrawScope(mMemGr, 2, plotMemory, d, Xmin, Xmax, Mmin, Mmax);
}
