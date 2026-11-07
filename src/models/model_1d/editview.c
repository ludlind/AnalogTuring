#include <raylib.h>
#include <math.h>

#include "gui/raygui.h"
#include "gui/visual.h"
#include "models/model_1d/program_common.h"
#include "models/model_1d/editview.h"

static Rectangle StateMap     = { 65, 30, 260, 260 };
static Rectangle MemoryMap    = { 65, 380, 260, 260 };
static Rectangle StateSlider  = { 460, 20, 300, 20 };
static Rectangle FSSlider     = { 460, 50, 300, 20 };
static Rectangle FXSlider     = { 460, 80, 300, 20 };
static Rectangle FMSlider     = { 460, 110, 300, 20 };
static Rectangle StartSSlider = { 460, 170, 300, 20 };
static Rectangle DtSlider     = { 460, 200, 300, 20 };
static Rectangle SigmaMSlider = { 460, 230, 300, 20 };

static const PlotAxesT stateMapAxes = {
    "State map", "S", "m",
    STATE_MIN_VALUE, STATE_MAX_VALUE, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
    false,
};
static const PlotAxesT initTapeAxes = {
    "Initial tape M0(x)", "x", "M",
    MEMORY_START_POSITION, MEMORY_STOP_POSITION, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE,
    false,
};

static float plotStateMap(const void *ctx, float S, float m)
{
    float Snorm = (S - STATE_MIN_VALUE)/(STATE_MAX_VALUE - STATE_MIN_VALUE);
    float mnorm = (m - MEMORY_MIN_VALUE)/(MEMORY_MAX_VALUE - MEMORY_MIN_VALUE);
    return model1d_StateMapGetValueF(ctx, Snorm, mnorm);
}

static float plotInitM(const void *ctx, float x)
{
    return model1d_interpInitM(ctx, x);
}

void model1d_DrawEditView(void *data)
{
    Model1DDataT *d = model1d_of(data);

    GuiSpinner(StateSlider, TextFormat("State %d", d->selectedState), &d->selectedState, 0, NUMBER_OF_STATES - 1, true);

    int s = d->selectedState;
    GuiSlider(FSSlider, TextFormat("F_S = %0.2f", d->FS[s]), NULL, &d->FS[s], -10.0f, 10.0f);
    GuiSlider(FXSlider, TextFormat("F_X = %0.2f", d->FX[s]), NULL, &d->FX[s], -10.0f, 10.0f);
    GuiSlider(FMSlider, TextFormat("F_M = %0.2f", d->FM[s]), NULL, &d->FM[s], -10.0f, 10.0f);
    GuiSlider(StartSSlider, TextFormat("S_Start = %0.2f", d->startS), NULL, &d->startS, -1.0f, 1.0f);

    float logDt = log10f(d->parameters.dt);
    float logSigmaM = log10f(d->parameters.sigma_M);
    GuiSlider(DtSlider, TextFormat("Delta T (log) = %0.2f", logDt), NULL, &logDt, -10.0f, 10.0f);
    GuiSlider(SigmaMSlider, TextFormat("Sigma_M (log) = %0.2f", logSigmaM), NULL, &logSigmaM, -10.0f, 10.0f);
    d->parameters.dt = powf(10.0f, logDt);
    d->parameters.sigma_M = powf(10.0f, logSigmaM);

    DrawMap(StateMap, 5, 2, 0, plotStateMap, d, stateMapAxes, 0.0f, (float)NUMBER_OF_STATES);
    DrawDot(StateMap, 2, d->startS, model1d_interpInitM(d, 0.0f), stateMapAxes, BLACK);

    DrawScope(MemoryMap, 2, plotInitM, d, initTapeAxes);
}

void model1d_HandleInputEditView(void *data)
{
    Model1DDataT *d = model1d_of(data);

    Vector2 mousePos = GetMousePosition();

    float Xmin = MEMORY_START_POSITION;
    float Xmax = MEMORY_STOP_POSITION;

    if (CheckCollisionPointRec(mousePos, StateMap))
    {
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

        float x = ((float)mousePos.x - StateMap.x - 2.0f)/(StateMap.width - 4.0f);
        float y = 1.0f - ((float)mousePos.y - StateMap.y - 2.0f)/(StateMap.height - 4.0f);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            model1d_StateMapSetValue(d, x, y, d->selectedState);
        }

        float wheel = GetMouseWheelMove();
        if (wheel > 0)
        {
            d->selectedState = (int)clamp(d->selectedState + 1, 0, NUMBER_OF_STATES - 1);
        }
        else if (wheel < 0)
        {
            d->selectedState = (int)clamp(d->selectedState - 1, 0, NUMBER_OF_STATES - 1);
        }
    }
    else if (CheckCollisionPointRec(mousePos, MemoryMap))
    {
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

        float x = ((float)mousePos.x - MemoryMap.x - 2.0f)/(MemoryMap.width - 4.0f);
        float y = 1.0f - ((float)mousePos.y - MemoryMap.y - 2.0f)/(MemoryMap.height - 4.0f);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            float sigma = powf(10.0f, d->brushSize);
            model1d_writeGaussianInitM(d, Xmin + (Xmax - Xmin)*x, sigma*(0.2f*y - 0.1f), sigma);
        }

        float wheel = GetMouseWheelMove();
        if (wheel > 0)
        {
            d->brushSize += 0.1f;
        }
        else if (wheel < 0)
        {
            d->brushSize -= 0.1f;
        }
    }
}
