#include <math.h>
#include <complex.h>
#include <raylib.h>

#include "gui/raygui.h"
#include "gui/visual.h"
#include "models/model_2d/keyframe_program.h"
#include "models/model_2d/tape.h"
#include "models/model_2d/inputview.h"

#define LABEL_X       80
#define SLIDER_X      240
#define SLIDER_WIDTH  300
#define ROW_HEIGHT    20

static Rectangle tapeMap      = { 80, 80, 860, 300 };
static Rectangle dtSlider     = { SLIDER_X, 460, SLIDER_WIDTH, ROW_HEIGHT };
static Rectangle sigmaMSlider = { SLIDER_X, 495, SLIDER_WIDTH, ROW_HEIGHT };
static Rectangle omegaSlider  = { SLIDER_X, 530, SLIDER_WIDTH, ROW_HEIGHT };
static Rectangle startReSlider = { SLIDER_X, 565, SLIDER_WIDTH, ROW_HEIGHT };
static Rectangle startImSlider = { SLIDER_X, 600, SLIDER_WIDTH, ROW_HEIGHT };

static const Color symbolLineColor = { 255, 200, 80, 110 };
static const Color brushColor      = { 255, 255, 255, 140 };

static const PlotAxesT tapeAxes = {
    "Initial tape M0(x)", "x", "M",
    MEMORY_START_POSITION, MEMORY_STOP_POSITION, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
    false,
};

static float plotInitM(const void *ctx, float x)
{
    return model2d_InterpInitM(ctx, x);
}

static bool tapeAt(Vector2 mouse, float *x, float *y)
{
    if (!CheckCollisionPointRec(mouse, tapeMap))
    {
        return false;
    }
    *x = (mouse.x - tapeMap.x - 2.0f)/(tapeMap.width - 4.0f);
    *y = 1.0f - (mouse.y - tapeMap.y - 2.0f)/(tapeMap.height - 4.0f);
    return true;
}

static float tapePosition(float u)
{
    return tapeAxes.xmin + u*(tapeAxes.xmax - tapeAxes.xmin);
}

static void drawLabel(const char *text, Rectangle row)
{
    DrawText(text, LABEL_X, row.y + 2, 16, LIGHTGRAY);
}

static void drawParameters(Model2DDataT *d)
{
    Model2DParametersT *p = &d->parameters;

    float logDt = log10f(p->dt);
    float logSigmaM = log10f(p->sigma_M);
    float startRe = crealf(d->startS);
    float startIm = cimagf(d->startS);

    drawLabel("dt (log)", dtSlider);
    GuiSlider(dtSlider, NULL, TextFormat("%0.2f", logDt), &logDt, -6.0f, -1.0f);
    drawLabel("sigma_M (log)", sigmaMSlider);
    GuiSlider(sigmaMSlider, NULL, TextFormat("%0.2f", logSigmaM), &logSigmaM, -4.0f, 0.0f);
    drawLabel("omega", omegaSlider);
    GuiSlider(omegaSlider, NULL, TextFormat("%0.2f", p->omega), &p->omega, 0.0f, 4.0f*PI);
    drawLabel("S_start Re", startReSlider);
    GuiSlider(startReSlider, NULL, TextFormat("%0.2f", startRe), &startRe, -STATE_MAX_RADIUS, STATE_MAX_RADIUS);
    drawLabel("S_start Im", startImSlider);
    GuiSlider(startImSlider, NULL, TextFormat("%0.2f", startIm), &startIm, -STATE_MAX_RADIUS, STATE_MAX_RADIUS);

    p->dt = powf(10.0f, logDt);
    p->sigma_M = powf(10.0f, logSigmaM);
    d->startS = startRe + I*startIm;

    DrawText("Parameters take effect on reset (R in Simulation or Slice)", LABEL_X, 645, 16, GRAY);
}

void model2d_InitInputView(InputViewT *input)
{
    input->brushSize = -3.0f;
}

void model2d_DrawInputView(void *data)
{
    Model2DDataT *d = model2d_of(data);

    DrawScope(tapeMap, 2, plotInitM, d, tapeAxes);

    for (int s = 0; s < d->program.symbols; s++)
    {
        DrawHorizontalLine(tapeMap, 2, model2d_SymbolLevel(&d->program, s), tapeAxes, symbolLineColor);
    }

    float u, v;
    if (tapeAt(GetMousePosition(), &u, &v))
    {
        float X = tapePosition(u);
        float sigma = powf(10.0f, d->input.brushSize);
        DrawVerticalLine(tapeMap, 2, X - sigma, tapeAxes, brushColor);
        DrawVerticalLine(tapeMap, 2, X + sigma, tapeAxes, brushColor);
    }

    DrawText("Drag: paint (above/below middle adds/removes)   Wheel: brush width",
             tapeMap.x, tapeMap.y + tapeMap.height + 30, 16, GRAY);

    drawParameters(d);
}

void model2d_HandleInputInputView(void *data)
{
    Model2DDataT *d = model2d_of(data);

    float u, v;
    if (!tapeAt(GetMousePosition(), &u, &v))
    {
        return;
    }
    SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

    float sigma = powf(10.0f, d->input.brushSize);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        model2d_WriteGaussianInitM(d, tapePosition(u), sigma*(0.2f*v - 0.1f), sigma);
    }

    float wheel = GetMouseWheelMove();
    if (wheel > 0)
    {
        d->input.brushSize += 0.1f;
    }
    else if (wheel < 0)
    {
        d->input.brushSize -= 0.1f;
    }
}
