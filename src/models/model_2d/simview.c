#include <math.h>
#include <complex.h>
#include <raylib.h>

#include "gui/visual.h"
#include "models/model_2d/solver.h"
#include "models/model_2d/simview.h"

static Rectangle FSPolarGr  = { 65, 60, 220, 220 };
static Rectangle FSColorBar = { 293, 60, 12, 220 };
static Rectangle FMPolarGr  = { 385, 60, 220, 220 };
static Rectangle FMColorBar = { 613, 60, 12, 220 };
static Rectangle FXPolarGr  = { 705, 60, 220, 220 };
static Rectangle FXColorBar = { 933, 60, 12, 220 };
static Rectangle SHistoryGr = { 65, 380, 220, 200 };
static Rectangle SColorBar  = { 293, 380, 12, 200 };
static Rectangle XHistoryGr = { 385, 380, 220, 200 };
static Rectangle mHistoryGr = { 705, 380, 220, 200 };

static const Color phaseMarkColor = { 200, 200, 200, 120 };

static const PlotAxesT SHistoryAxes = {
    "S(t)", "t", "|S|",
    0.0f, 0.0f, 0.0f, STATE_MAX_RADIUS,
    .hideXGrid = true,
};
static const PlotAxesT XHistoryAxes = {
    "X(t)", "t", "X",
    0.0f, 0.0f, MEMORY_START_POSITION, MEMORY_STOP_POSITION,
    .hideXGrid = true,
};
static const PlotAxesT mHistoryAxes = {
    "m(t)", "t", "m",
    0.0f, 0.0f, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
    .hideXGrid = true,
};

static float complex plotFS(const void *ctx, float complex S)
{
    const Model2DDataT *d = ctx;
    return d->velocity->F_S(d, S, d->state.m, d->state.theta);
}

static float plotFM(const void *ctx, float complex S)
{
    const Model2DDataT *d = ctx;
    return d->velocity->F_M(d, S, d->state.m, d->state.theta);
}

static float plotFX(const void *ctx, float complex S)
{
    const Model2DDataT *d = ctx;
    return d->velocity->F_X(d, S, d->state.m, d->state.theta);
}

static Color fieldColor(float v)
{
    return ColorMap((v - FIELD_MIN)/(FIELD_MAX - FIELD_MIN));
}

typedef struct SHistoryScopeS {
    const HistoryBinsT *bins;
    const float complex *history;
} SHistoryScopeT;

static bool plotSHistory(const void *ctx, int column, ScopeRangeT *range)
{
    const SHistoryScopeT *scope = ctx;
    int first, last;
    if (!historyColumnSamples(scope->bins, column, &first, &last))
    {
        return false;
    }

    float complex largest = scope->history[historySlot(scope->bins, first)];
    range->low = range->high = cabsf(largest);
    for (int sample = first + 1; sample <= last; sample++)
    {
        float complex S = scope->history[historySlot(scope->bins, sample)];
        float magnitude = cabsf(S);
        if (magnitude < range->low)
        {
            range->low = magnitude;
        }
        if (magnitude > range->high)
        {
            range->high = magnitude;
            largest = S;
        }
    }
    range->color = ColorAngle(cargf(largest));
    return true;
}

static PlotAxesT withTimeRange(PlotAxesT axes, float t1, float t2)
{
    axes.xmin = t1;
    axes.xmax = t2;
    return axes;
}

static void drawPhaseMarks(Rectangle rect, PlotAxesT axes, float omega)
{
    if (omega <= 0.0f)
    {
        return;
    }

    float period = 2.0f*PI/omega;
    int kmin = (int)ceilf(fmaxf(axes.xmin, 0.0f)/period);
    int kmax = (int)floorf(axes.xmax/period);
    for (int k = kmin; k <= kmax; k++)
    {
        DrawVerticalLine(rect, 2, k*period, axes, phaseMarkColor);
    }
}

void model2d_DrawSimulationView(void *data)
{
    Model2DDataT *d = model2d_of(data);

    HistoryBinsT bins = historyBins(d->state.steps, TIME_MEMORY_GRID_POINTS, ScopeColumns(SHistoryGr, 2));
    SHistoryScopeT SScope = { &bins, d->memory.SMem };
    float t1 = historyWindowStart(&bins, d->state.dt);
    float t2 = historyWindowEnd(&bins, d->state.dt);
    float omega = d->state.omega;

    PlotAxesT SAxes = withTimeRange(SHistoryAxes, t1, t2);
    PlotAxesT XAxes = withTimeRange(XHistoryAxes, t1, t2);
    PlotAxesT mAxes = withTimeRange(mHistoryAxes, t1, t2);

    DrawRangeScope(SHistoryGr, 2, plotSHistory, &SScope, SAxes);
    DrawPolarMap(FSPolarGr, 20, 64, plotFS, d, "F_S(S; theta, m)", STATE_MAX_RADIUS, 1.0f);
    DrawPolarDot(FSPolarGr, STATE_MAX_RADIUS, d->state.S, RAYWHITE);
    DrawColorBar(FSColorBar, 1, ColorAngle, "arg F_S", -PI, PI);

    DrawPolarScalarMap(FMPolarGr, 20, 64, plotFM, d, "F_M(S; theta, m)", STATE_MAX_RADIUS, FIELD_MIN, FIELD_MAX);
    DrawPolarDot(FMPolarGr, STATE_MAX_RADIUS, d->state.S, RAYWHITE);
    DrawColorBar(FMColorBar, 1, fieldColor, "F_M", FIELD_MIN, FIELD_MAX);

    DrawPolarScalarMap(FXPolarGr, 20, 64, plotFX, d, "F_X(S; theta, m)", STATE_MAX_RADIUS, FIELD_MIN, FIELD_MAX);
    DrawPolarDot(FXPolarGr, STATE_MAX_RADIUS, d->state.S, RAYWHITE);
    DrawColorBar(FXColorBar, 1, fieldColor, "F_X", FIELD_MIN, FIELD_MAX);

    DrawColorBar(SColorBar, 1, ColorAngle, "arg S", -PI, PI);
    DrawHistoryScope(XHistoryGr, 2, &bins, d->memory.XMem, XAxes);
    DrawHistoryScope(mHistoryGr, 2, &bins, d->memory.mMem, mAxes);

    drawPhaseMarks(SHistoryGr, SAxes, omega);
    drawPhaseMarks(XHistoryGr, XAxes, omega);
    drawPhaseMarks(mHistoryGr, mAxes, omega);
}
