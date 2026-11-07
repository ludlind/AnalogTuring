#include <math.h>
#include <complex.h>
#include <raylib.h>

#include "gui/visual.h"
#include "models/model_2d/solver.h"
#include "models/model_2d/simview.h"

static Rectangle SHistoryGr = { 65, 380, 220, 200 };
static Rectangle SColorBar  = { 293, 380, 12, 200 };
static Rectangle XHistoryGr = { 385, 380, 260, 200 };
static Rectangle mHistoryGr = { 705, 380, 260, 200 };

static const Color phaseMarkColor = { 200, 200, 200, 120 };

static const PlotAxesT SHistoryAxes = {
    "S(t)", "t", "|S|",
    0.0f, 0.0f, 0.0f, STATE_MAX_RADIUS,
};
static const PlotAxesT XHistoryAxes = {
    "X(t)", "t", "X",
    0.0f, 0.0f, MEMORY_START_POSITION, MEMORY_STOP_POSITION,
};
static const PlotAxesT mHistoryAxes = {
    "m(t)", "t", "m",
    0.0f, 0.0f, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
};

static Color angleColor(float angle)
{
    float hue = (angle + PI)/(2.0f*PI)*360.0f;
    return ColorFromHSV(hue, 0.8f, 1.0f);
}

static float plotSAbsHistory(const void *ctx, float t)
{
    return cabsf(model2d_interpS(ctx, t));
}

static Color plotSArgHistory(const void *ctx, float t)
{
    return angleColor(cargf(model2d_interpS(ctx, t)));
}

static float plotXHistory(const void *ctx, float t)
{
    return model2d_interpX(ctx, t);
}

static float plotmHistory(const void *ctx, float t)
{
    return model2d_interpm(ctx, t);
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

    float t1 = model2d_getMemoryStartTime(d);
    float t2 = d->state.t;
    float omega = d->state.omega;

    PlotAxesT SAxes = withTimeRange(SHistoryAxes, t1, t2);
    PlotAxesT XAxes = withTimeRange(XHistoryAxes, t1, t2);
    PlotAxesT mAxes = withTimeRange(mHistoryAxes, t1, t2);

    DrawColoredScope(SHistoryGr, 2, plotSAbsHistory, plotSArgHistory, d, SAxes);
    DrawColorBar(SColorBar, 1, angleColor, "arg S", -PI, PI);
    DrawScope(XHistoryGr, 2, plotXHistory, d, XAxes);
    DrawScope(mHistoryGr, 2, plotmHistory, d, mAxes);

    drawPhaseMarks(SHistoryGr, SAxes, omega);
    drawPhaseMarks(XHistoryGr, XAxes, omega);
    drawPhaseMarks(mHistoryGr, mAxes, omega);
}
