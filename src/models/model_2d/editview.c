#include <math.h>
#include <complex.h>
#include <raylib.h>

#include "gui/raygui.h"
#include "gui/visual.h"
#include "gui/valuerow.h"
#include "lib/util.h"
#include "models/model_2d/keyframe_program.h"
#include "models/model_2d/editview.h"

#define SIGMA_MIN        0.01f
#define SIGMA_MAX        2.0f
#define SIGMA_WHEEL_STEP 1.1f
#define PICK_PIXELS      8.0f
#define FS_MAGNITUDE_MAX 5.0f
#define LINEAR_MAX       20.0f

#define LABEL_X       500
#define CONTROL_X     600
#define CONTROL_WIDTH 200
#define ROW_HEIGHT    20

#define PARAM_ROWS    9

static Rectangle splatMap       = { 80, 80, 300, 300 };
static Rectangle splatColorBar  = { 400, 80, 12, 300 };
static Rectangle functionPicker = { CONTROL_X, 80, CONTROL_WIDTH, ROW_HEIGHT };
static Rectangle componentPicker = { CONTROL_X, 115, CONTROL_WIDTH/3, ROW_HEIGHT };
static Rectangle indexGrid      = { 80, 500, 300, 240 };
static Rectangle functionSwatch  = { CONTROL_X + CONTROL_WIDTH + 8, 80, ROW_HEIGHT, ROW_HEIGHT };
static Rectangle keyframePicker  = { CONTROL_X, 500, CONTROL_WIDTH, ROW_HEIGHT };
static Rectangle symbolPicker    = { CONTROL_X, 535, CONTROL_WIDTH, ROW_HEIGHT };

static const Color functionColors[NUMBER_OF_SPLAT_FUNCTIONS] = {
    { 70, 75, 90, 255 },
    { 0, 121, 241, 255 },
    { 255, 161, 0, 255 },
    { 0, 158, 47, 255 },
    { 230, 41, 55, 255 },
    { 135, 60, 190, 255 },
    { 0, 190, 190, 255 },
    { 211, 176, 131, 255 },
};

static ValueBoxT paramBoxes[PARAM_ROWS];

static const ValueRowLayoutT paramLayout = {
    .labelX = LABEL_X,
    .sliderX = CONTROL_X,
    .sliderWidth = 130,
    .boxWidth = 60,
    .y = 190,
    .spacing = 28,
    .height = ROW_HEIGHT,
};

static const char *componentTitles[] = {
    [FIELD_F_S] = "F_S splats",
    [FIELD_F_M] = "F_M splats",
    [FIELD_F_X] = "F_X splats",
};

static SplatListT *currentList(Model2DDataT *d)
{
    KeyframeEditT *edit = &d->edit;
    return &d->program.functions[edit->selectedFunction].components[edit->component];
}

static SplatT *selectedSplat(Model2DDataT *d)
{
    SplatListT *list = currentList(d);
    int k = d->edit.selectedSplat;
    if (k < 0 || k >= list->count)
    {
        return NULL;
    }
    return &list->splats[k];
}

static float complex clampToDisk(float complex z)
{
    float r = cabsf(z);
    if (r > STATE_MAX_RADIUS)
    {
        return z*(STATE_MAX_RADIUS/r);
    }
    return z;
}

static float complex plotSplatsComplex(const void *ctx, float complex S)
{
    return model2d_EvalSplats(ctx, S);
}

static float plotSplatsReal(const void *ctx, float complex S)
{
    return crealf(model2d_EvalSplats(ctx, S));
}

static Color fieldColor(float v)
{
    return ColorMap((v - FIELD_MIN)/(FIELD_MAX - FIELD_MIN));
}

static int pickSplat(const SplatListT *list, Vector2 mouse)
{
    int best = -1;
    float bestDistance = PICK_PIXELS;
    float scale = PolarPixelsPerUnit(splatMap, STATE_MAX_RADIUS);
    float complex z;
    PolarScreenToValue(splatMap, STATE_MAX_RADIUS, mouse, &z);

    for (int k = 0; k < list->count; k++)
    {
        float distance = cabsf(list->splats[k].center - z)*scale;
        if (distance <= bestDistance)
        {
            best = k;
            bestDistance = distance;
        }
    }
    return best;
}

static void drawSplatMarkers(const SplatListT *list, int selected)
{
    for (int k = 0; k < list->count; k++)
    {
        const SplatT *splat = &list->splats[k];
        Color color = (k == selected) ? YELLOW : RAYWHITE;
        DrawPolarDot(splatMap, STATE_MAX_RADIUS, splat->center, color);
        DrawPolarRing(splatMap, STATE_MAX_RADIUS, splat->center, splat->sigma, Fade(color, 0.6f));
    }
}

static void paramRow(int row, const char *label, float *value, float min, float max)
{
    DrawValueRow(&paramLayout, row, &paramBoxes[row], label, value, min, max);
}

static int linearControls(int row, SplatT *target)
{
    float attract = -crealf(target->alpha);
    float swirl = cimagf(target->alpha);
    float saddle = cabsf(target->beta);
    float saddleAngle = cargf(target->beta);

    paramRow(row++, "Attract", &attract, -LINEAR_MAX, LINEAR_MAX);
    paramRow(row++, "Swirl", &swirl, -LINEAR_MAX, LINEAR_MAX);
    paramRow(row++, "Saddle", &saddle, 0.0f, LINEAR_MAX);
    paramRow(row++, "Saddle arg", &saddleAngle, -PI, PI);

    target->alpha = -attract + I*swirl;
    target->beta = fmaxf(saddle, 0.0f)*cexpf(I*saddleAngle);
    return row;
}

static void drawValueControls(Model2DDataT *d, SplatT *splat)
{
    KeyframeEditT *edit = &d->edit;
    SplatT *target = splat ? splat : &edit->brush[edit->component];
    int row = 0;

    if (edit->component == FIELD_F_S)
    {
        float magnitude = cabsf(target->value);
        float angle = cargf(target->value);
        paramRow(row++, "|F|", &magnitude, 0.0f, FS_MAGNITUDE_MAX);
        paramRow(row++, "arg F", &angle, -PI, PI);
        target->value = fmaxf(magnitude, 0.0f)*cexpf(I*angle);
        row = linearControls(row, target);
    }
    else
    {
        float value = crealf(target->value);
        paramRow(row++, "F", &value, FIELD_MIN, FIELD_MAX);
        target->value = value;
    }

    paramRow(row++, "Width", &target->sigma, SIGMA_MIN, SIGMA_MAX);
    target->sigma = clamp(target->sigma, SIGMA_MIN, SIGMA_MAX);

    if (splat == NULL)
    {
        return;
    }

    float re = crealf(splat->center);
    float im = cimagf(splat->center);
    paramRow(row++, "Center Re", &re, -STATE_MAX_RADIUS, STATE_MAX_RADIUS);
    paramRow(row++, "Center Im", &im, -STATE_MAX_RADIUS, STATE_MAX_RADIUS);
    splat->center = clampToDisk(re + I*im);

    edit->brush[edit->component] = *splat;
}

static float cellWidth(const KeyframeProgramT *program)
{
    return indexGrid.width/program->keyframes;
}

static float cellHeight(const KeyframeProgramT *program)
{
    return indexGrid.height/program->symbols;
}

static Rectangle cellRect(const KeyframeProgramT *program, int keyframe, int symbol)
{
    float w = cellWidth(program);
    float h = cellHeight(program);
    int row = program->symbols - 1 - symbol;
    return (Rectangle){ indexGrid.x + keyframe*w, indexGrid.y + row*h, w, h };
}

static bool cellAt(const KeyframeProgramT *program, Vector2 p, int *keyframe, int *symbol)
{
    if (!CheckCollisionPointRec(p, indexGrid))
    {
        return false;
    }
    int column = (int)((p.x - indexGrid.x)/cellWidth(program));
    int row = (int)((p.y - indexGrid.y)/cellHeight(program));
    *keyframe = MIN(column, program->keyframes - 1);
    *symbol = program->symbols - 1 - MIN(row, program->symbols - 1);
    return true;
}

static void drawIndexGrid(const KeyframeProgramT *program, int selectedFunction)
{
    for (int k = 0; k < program->keyframes; k++)
    {
        for (int s = 0; s < program->symbols; s++)
        {
            int index = program->index[k][s];
            Rectangle cell = cellRect(program, k, s);
            DrawRectangleRec(cell, functionColors[index]);
            DrawRectangleLinesEx(cell, 1, (Color){ 8, 10, 14, 255 });

            const char *label = TextFormat("%d", index);
            int w = MeasureText(label, 16);
            DrawText(label, cell.x + (cell.width - w)/2, cell.y + (cell.height - 16)/2, 16, RAYWHITE);
        }
    }

    for (int k = 0; k < program->keyframes; k++)
    {
        for (int s = 0; s < program->symbols; s++)
        {
            if (program->index[k][s] == selectedFunction)
            {
                DrawRectangleLinesEx(cellRect(program, k, s), 2, YELLOW);
            }
        }
    }

    DrawRectangleLinesEx(indexGrid, 2, RAYWHITE);

    const char *title = "Function per keyframe and symbol";
    DrawText(title, indexGrid.x + (indexGrid.width - MeasureText(title, 20))/2, indexGrid.y - 44, 20, RAYWHITE);

    float below = indexGrid.y + indexGrid.height + 4;
    int labelStep = program->keyframes > 8 ? 2 : 1;
    for (int k = 0; k < program->keyframes; k += labelStep)
    {
        Rectangle cell = cellRect(program, k, 0);
        const char *label = TextFormat("%.2f", model2d_KeyframeTheta(program, k));
        DrawText(label, cell.x + (cell.width - MeasureText(label, 14))/2, below, 14, LIGHTGRAY);
    }
    const char *xlabel = "theta";
    DrawText(xlabel, indexGrid.x + (indexGrid.width - MeasureText(xlabel, 16))/2, below + 18, 16, LIGHTGRAY);

    for (int s = 0; s < program->symbols; s++)
    {
        Rectangle cell = cellRect(program, 0, s);
        const char *label = TextFormat("%.2f", model2d_SymbolLevel(program, s));
        DrawText(label, indexGrid.x - MeasureText(label, 14) - 6, cell.y + (cell.height - 14)/2, 14, LIGHTGRAY);
    }
    DrawText("m", indexGrid.x - 60, indexGrid.y - 20, 16, LIGHTGRAY);
}

static void drawGridControls(KeyframeProgramT *program)
{
    DrawText("Keyframes", LABEL_X, keyframePicker.y + 2, 16, LIGHTGRAY);
    GuiSpinner(keyframePicker, NULL, &program->keyframes, 1, MAX_KEYFRAMES, false);

    DrawText("Symbols", LABEL_X, symbolPicker.y + 2, 16, LIGHTGRAY);
    GuiSpinner(symbolPicker, NULL, &program->symbols, 1, MAX_SYMBOLS, false);

    DrawText("Grid: drag to assign function", LABEL_X, 580, 16, GRAY);
    DrawText("Right click: pick up function", LABEL_X, 600, 16, GRAY);
}

static void handleGridInput(Model2DDataT *d, Vector2 mouse)
{
    KeyframeProgramT *program = &d->program;
    KeyframeEditT *edit = &d->edit;

    int keyframe, symbol;
    if (!cellAt(program, mouse, &keyframe, &symbol))
    {
        return;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !edit->dragging)
    {
        program->index[keyframe][symbol] = edit->selectedFunction;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        edit->selectedFunction = program->index[keyframe][symbol];
        edit->selectedSplat = -1;
    }
}

void model2d_InitKeyframeEdit(KeyframeEditT *edit)
{
    edit->selectedFunction = 0;
    edit->component = FIELD_F_S;
    edit->selectedSplat = -1;
    edit->dragging = false;
    for (int c = 0; c < FIELD_COMPONENT_COUNT; c++)
    {
        edit->brush[c] = (SplatT){ .sigma = 0.2f, .value = 0.5f };
    }
}

void model2d_DrawEditView(void *data)
{
    Model2DDataT *d = model2d_of(data);
    KeyframeEditT *edit = &d->edit;
    SplatListT *list = currentList(d);

    if (edit->component == FIELD_F_S)
    {
        DrawPolarMap(splatMap, 30, 96, plotSplatsComplex, list,
                     componentTitles[edit->component], STATE_MAX_RADIUS, 1.0f);
        DrawColorBar(splatColorBar, 1, ColorAngle, "arg F", -PI, PI);
    }
    else
    {
        DrawPolarScalarMap(splatMap, 30, 96, plotSplatsReal, list,
                           componentTitles[edit->component], STATE_MAX_RADIUS, FIELD_MIN, FIELD_MAX);
        DrawColorBar(splatColorBar, 1, fieldColor, "F", FIELD_MIN, FIELD_MAX);
    }
    drawSplatMarkers(list, edit->selectedSplat);

    drawIndexGrid(&d->program, edit->selectedFunction);
    drawGridControls(&d->program);

    int function = edit->selectedFunction;
    int component = edit->component;

    DrawText("Function", LABEL_X, functionPicker.y + 2, 16, LIGHTGRAY);
    GuiSpinner(functionPicker, NULL, &edit->selectedFunction, 0, NUMBER_OF_SPLAT_FUNCTIONS - 1, false);
    DrawRectangleRec(functionSwatch, functionColors[edit->selectedFunction]);
    DrawRectangleLinesEx(functionSwatch, 1, RAYWHITE);

    DrawText("Component", LABEL_X, componentPicker.y + 2, 16, LIGHTGRAY);
    GuiToggleGroup(componentPicker, "F_S;F_M;F_X", &edit->component);

    if (function != edit->selectedFunction || component != edit->component)
    {
        edit->selectedSplat = -1;
        edit->dragging = false;
        return;
    }

    SplatT *splat = selectedSplat(d);
    const char *status = splat
        ? TextFormat("Gaussian %d of %d", edit->selectedSplat + 1, list->count)
        : TextFormat("New Gaussian (%d of %d placed)", list->count, MAX_SPLATS);
    DrawText(status, LABEL_X, 160, 16, RAYWHITE);

    drawValueControls(d, splat);

    DrawText("Click: add/select   Drag: move", LABEL_X, 640, 16, GRAY);
    DrawText("Wheel: width   Delete: remove", LABEL_X, 660, 16, GRAY);
}

void model2d_HandleInputEditView(void *data)
{
    Model2DDataT *d = model2d_of(data);
    KeyframeEditT *edit = &d->edit;
    SplatListT *list = currentList(d);
    Vector2 mouse = GetMousePosition();

    handleGridInput(d, mouse);

    float complex z;
    bool inside = PolarScreenToValue(splatMap, STATE_MAX_RADIUS, mouse, &z);

    if (inside)
    {
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    }

    if (inside && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        int k = pickSplat(list, mouse);
        if (k < 0)
        {
            SplatT splat = edit->brush[edit->component];
            splat.center = z;
            k = model2d_AddSplat(list, splat);
        }
        edit->selectedSplat = k;
        edit->dragging = (k >= 0);
    }

    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        edit->dragging = false;
    }

    SplatT *splat = selectedSplat(d);
    if (splat == NULL)
    {
        return;
    }

    if (edit->dragging)
    {
        splat->center = clampToDisk(z);
    }

    float wheel = GetMouseWheelMove();
    if (inside && wheel != 0.0f)
    {
        splat->sigma = clamp(splat->sigma*powf(SIGMA_WHEEL_STEP, wheel), SIGMA_MIN, SIGMA_MAX);
    }

    if (IsKeyPressed(KEY_DELETE) && !AnyValueBoxEditing(paramBoxes, PARAM_ROWS))
    {
        model2d_RemoveSplat(list, edit->selectedSplat);
        edit->selectedSplat = -1;
        edit->dragging = false;
    }
}
