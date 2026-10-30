#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "raygui.h"

#include "state.h"
#include "program.h"
#include "models/common.h"
#include "visual.h"
#include "editview.h"

static Rectangle StateMap     = { 20, 20, 300, 300 };
static Rectangle MemoryMap    = { 20, 340, 300, 300 };
static Rectangle StateSlider  = { 440, 20, 300, 20 };
static Rectangle FSSlider     = { 440, 50, 300, 20 };
static Rectangle FXSlider     = { 440, 80, 300, 20 };
static Rectangle FMSlider     = { 440, 110, 300, 20 };
static Rectangle StartSSlider = { 440, 170, 300, 20 };

static int state = 0;

void DrawEditView(ProgramT* program_p)
{
    float Smin = getStateMinValue();
    float Smax = getStateMaxValue();
    
    float Xmin = getMemoryStartPosition();
    float Xmax = getMemoryStopPosition();

    float Mmin = getMemoryMinValue();
    float Mmax = getMemoryMaxValue();

    GuiSpinner(StateSlider, TextFormat("State %d", state), &state, 0, NUMBER_OF_STATES - 1, true);
    
    float value1 = GetFSValue(state);
    float value2 = GetFXValue(state);
    float value3 = GetFMValue(state);
    float value4 = GetStartS();
    
    GuiSlider(FSSlider, TextFormat("F_S = %0.2f", value1), NULL, &value1, -10.0f, 10.0f);
    GuiSlider(FXSlider, TextFormat("F_X = %0.2f", value2), NULL, &value2, -10.0f, 10.0f);
    GuiSlider(FMSlider, TextFormat("F_M = %0.2f", value3), NULL, &value3, -10.0f, 10.0f);
    GuiSlider(StartSSlider, TextFormat("S_Start = %0.2f", value4), NULL, &value4, -1.0f, 1.0f);

    SetFSValue(state, value1);
    SetFXValue(state, value2);
    SetFMValue(state, value3);
    SetStartS(value4);
    
    DrawMap(StateMap, 5, 2, 0, StateMapGetValueF, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f*NUMBER_OF_STATES);
    DrawDot(StateMap, 2, program_p->startS, interpInitM(0.0f), Smin, Smax, Mmin, Mmax, BLACK);
    
    DrawScope(MemoryMap, 2, interpInitM, Xmin, Xmax, Mmin, Mmax);
}

void HandleInputEditView(AppT* app_p, ProgramT* program_p)
{
    static float brushSize = -3.0f;

    Vector2 mousePos = GetMousePosition();

    float Smin = getStateMinValue();
    float Smax = getStateMaxValue();
    
    float Xmin = getMemoryStartPosition();
    float Xmax = getMemoryStopPosition();

    float Mmin = getMemoryMinValue();
    float Mmax = getMemoryMaxValue();

    if (CheckCollisionPointRec(mousePos, StateMap))
    {
        app_p->cursor = MOUSE_CURSOR_CROSSHAIR;

        float x = ((float)mousePos.x - StateMap.x - 2.0f)/(StateMap.width - 4.0f);
        float y = 1.0f - ((float)mousePos.y - StateMap.y - 2.0f)/(StateMap.height - 4.0f);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            StateMapSetValue(x, y, state);
        }
    
        float wheel = GetMouseWheelMove();
        if (wheel > 0)
        {
            state = (int)clamp(state + 1, 0, NUMBER_OF_STATES);
        }
        else if (wheel < 0)
        {
            state = (int)clamp(state - 1, 0, NUMBER_OF_STATES);
        }
    }
    else if (CheckCollisionPointRec(mousePos, MemoryMap))
    {
        app_p->cursor = MOUSE_CURSOR_CROSSHAIR;

        float x = ((float)mousePos.x - MemoryMap.x - 2.0f)/(MemoryMap.width - 4.0f);
        float y = 1.0f - ((float)mousePos.y - MemoryMap.y - 2.0f)/(MemoryMap.height - 4.0f);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            float sigma = pow(10, brushSize);
            writeGaussianInitM(Xmin + (Xmax - Xmin)*x, sigma*(0.2f*y - 0.1f), sigma);
        }
    
        float wheel = GetMouseWheelMove();
        if (wheel > 0)
        {
            brushSize += 0.1f;
        }
        else if (wheel < 0)
        {
            brushSize -= 0.1f;
        }
    }
}
