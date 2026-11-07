#include <math.h>
#include <complex.h>
#include <raylib.h>

#include "gui/visual.h"
#include "models/model_2d/keyframe_program.h"
#include "models/model_2d/tape.h"
#include "models/model_2d/inputview.h"

static Rectangle tapeMap      = { 80, 80, 860, 300 };

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
