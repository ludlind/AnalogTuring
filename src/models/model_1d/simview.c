#include <raylib.h>

#include "gui/visual.h"
#include "models/model_1d/solver.h"
#include "models/model_1d/simview.h"

static Rectangle FSMapGr = { 65, 30, 260, 260 };
static Rectangle FMMapGr = { 385, 30, 260, 260 };
static Rectangle FXMapGr = { 705, 30, 260, 260 };
static Rectangle SMemGr  = { 65, 380, 260, 200 };
static Rectangle XMemGr  = { 385, 380, 260, 200 };
static Rectangle tapeGr  = { 705, 380, 260, 200 };

static const PlotAxesT FSAxes = {
    "F_S(S, m)", "S", "m",
    STATE_MIN_VALUE, STATE_MAX_VALUE, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
    false,
};
static const PlotAxesT FMAxes = {
    "F_M(S, m)", "S", "m",
    STATE_MIN_VALUE, STATE_MAX_VALUE, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
    false,
};
static const PlotAxesT FXAxes = {
    "F_X(S, m)", "S", "m",
    STATE_MIN_VALUE, STATE_MAX_VALUE, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
    false,
};
static const PlotAxesT SHistoryAxes = {
    "S(t)", "t", "S",
    0.0f, 0.0f, STATE_MIN_VALUE, STATE_MAX_VALUE,
    false,
};
static const PlotAxesT XHistoryAxes = {
    "X(t)", "t", "X",
    0.0f, 0.0f, MEMORY_START_POSITION, MEMORY_STOP_POSITION,
    false,
};
static const PlotAxesT tapeAxes = {
    "Tape M(x)", "x", "M",
    MEMORY_START_POSITION, MEMORY_STOP_POSITION, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
    false,
};

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

static float plotTape(const void *ctx, float x)
{
    return model1d_interpM(ctx, x);
}

static PlotAxesT withTimeRange(PlotAxesT axes, float t1, float t2)
{
    axes.xmin = t1;
    axes.xmax = t2;
    return axes;
}

void model1d_DrawSimulationView(void *data)
{
    Model1DDataT *d = model1d_of(data);

    float S = d->state.S;
    float m = d->state.m;

    HistoryBinsT bins = historyBins(d->state.steps, TIME_MEMORY_GRID_POINTS, ScopeColumns(SMemGr, 2));
    float t1 = historyWindowStart(&bins, d->state.dt);
    float t2 = historyWindowEnd(&bins, d->state.dt);

    DrawMap(FSMapGr, 5, 2, 0, plotFS, d, FSAxes, -1.0f, 1.0f);
    DrawMap(FMMapGr, 5, 2, 0, plotFM, d, FMAxes, -1.0f, 1.0f);
    DrawMap(FXMapGr, 5, 2, 0, plotFX, d, FXAxes, -1.0f, 1.0f);

    DrawDot(FSMapGr, 2, S, m, FSAxes, RAYWHITE);
    DrawDot(FMMapGr, 2, S, m, FMAxes, RAYWHITE);
    DrawDot(FXMapGr, 2, S, m, FXAxes, RAYWHITE);

    DrawHistoryScope(SMemGr, 2, &bins, d->memory.SMem, withTimeRange(SHistoryAxes, t1, t2));
    DrawHistoryScope(XMemGr, 2, &bins, d->memory.XMem, withTimeRange(XHistoryAxes, t1, t2));
    DrawScope(tapeGr, 2, plotTape, d, tapeAxes);
}
