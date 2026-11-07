#include <math.h>
#include <complex.h>
#include <stdbool.h>
#include <string.h>
#include <raylib.h>

#include "gui/raygui.h"
#include "gui/visual.h"
#include "models/model_2d/sliceview.h"

static Rectangle sliceMap      = { 65, 60, 480, 480 };
static Rectangle sliceColorBar = { 553, 60, 12, 480 };

#define CONTROL_LABEL_X 640
#define CONTROL_X       740
#define CONTROL_WIDTH   220
#define CONTROL_HEIGHT  20
#define SLIDER_X        790
#define SLIDER_WIDTH    170

static Rectangle xAxisPicker   = { CONTROL_X, 60,  CONTROL_WIDTH, CONTROL_HEIGHT };
static Rectangle yAxisPicker   = { CONTROL_X, 95,  CONTROL_WIDTH, CONTROL_HEIGHT };
static Rectangle fieldPicker   = { CONTROL_X, 130, CONTROL_WIDTH, CONTROL_HEIGHT };
static Rectangle systemPicker  = { CONTROL_X, 165, CONTROL_WIDTH/2, CONTROL_HEIGHT };
static Rectangle followBox     = { CONTROL_X, 200, CONTROL_HEIGHT, CONTROL_HEIGHT };

#define SLIDERS_Y       250
#define SLIDER_SPACING  35

typedef struct SliceVarInfoS {
    const char *name;
    float min, max;
} SliceVarInfoT;

static const SliceVarInfoT varInfo[SLICE_VAR_COUNT] = {
    [SLICE_RE_S]  = { "Re S",  -STATE_MAX_RADIUS, STATE_MAX_RADIUS },
    [SLICE_IM_S]  = { "Im S",  -STATE_MAX_RADIUS, STATE_MAX_RADIUS },
    [SLICE_ABS_S] = { "|S|",   0.0f, STATE_MAX_RADIUS },
    [SLICE_ARG_S] = { "arg S", -PI, PI },
    [SLICE_M]     = { "m",     MEMORY_MIN_VALUE, MEMORY_MAX_VALUE },
    [SLICE_THETA] = { "theta", 0.0f, 2.0f*PI },
};

static const char *fieldNames[] = {
    [SLICE_F_S] = "F_S",
    [SLICE_F_M] = "F_M",
    [SLICE_F_X] = "F_X",
};

static const char *fieldTitles[] = {
    [SLICE_F_S] = "F_S slice",
    [SLICE_F_M] = "F_M slice",
    [SLICE_F_X] = "F_X slice",
};

static bool isSVar(int v)
{
    return v == SLICE_RE_S || v == SLICE_IM_S || v == SLICE_ABS_S || v == SLICE_ARG_S;
}

static bool isPolarVar(int v)
{
    return v == SLICE_ABS_S || v == SLICE_ARG_S;
}

static bool yAllowed(int x, int y)
{
    if (x == y)
    {
        return false;
    }
    if (isSVar(x) && isSVar(y))
    {
        return isPolarVar(x) == isPolarVar(y);
    }
    return true;
}

static bool usesPolar(const SliceViewT *slice)
{
    if (isSVar(slice->xAxis))
    {
        return isPolarVar(slice->xAxis);
    }
    if (isSVar(slice->yAxis))
    {
        return isPolarVar(slice->yAxis);
    }
    return slice->polarSliders;
}

static bool isSlider(const SliceViewT *slice, int v)
{
    if (v == slice->xAxis || v == slice->yAxis)
    {
        return false;
    }
    if (isSVar(v))
    {
        return isPolarVar(v) == usesPolar(slice);
    }
    return true;
}

static bool pointAt(const SliceViewT *slice, float x, float y,
                    float complex *S, float *m, float *theta)
{
    float v[SLICE_VAR_COUNT];
    memcpy(v, slice->values, sizeof(v));
    v[slice->xAxis] = x;
    v[slice->yAxis] = y;

    if (usesPolar(slice))
    {
        *S = v[SLICE_ABS_S]*cexpf(I*v[SLICE_ARG_S]);
    }
    else
    {
        *S = v[SLICE_RE_S] + I*v[SLICE_IM_S];
    }
    *m = v[SLICE_M];
    *theta = v[SLICE_THETA];

    return cabsf(*S) <= STATE_MAX_RADIUS;
}

static float complex sliceFS(const void *ctx, float x, float y)
{
    const Model2DDataT *d = ctx;
    float complex S;
    float m, theta;
    if (!pointAt(&d->slice, x, y, &S, &m, &theta))
    {
        return NAN;
    }
    return d->velocity->F_S(d, S, m, theta);
}

static float sliceScalar(const void *ctx, float x, float y)
{
    const Model2DDataT *d = ctx;
    float complex S;
    float m, theta;
    if (!pointAt(&d->slice, x, y, &S, &m, &theta))
    {
        return NAN;
    }
    if (d->slice.field == SLICE_F_M)
    {
        return d->velocity->F_M(d, S, m, theta);
    }
    return d->velocity->F_X(d, S, m, theta);
}

static Color fieldColor(float v)
{
    return ColorMap((v - FIELD_MIN)/(FIELD_MAX - FIELD_MIN));
}

static const char *xAxisNames(void)
{
    static char names[128] = "";
    if (names[0] != '\0')
    {
        return names;
    }
    for (int v = 0; v < SLICE_VAR_COUNT; v++)
    {
        if (v > 0)
        {
            strcat(names, ";");
        }
        strcat(names, varInfo[v].name);
    }
    return names;
}

static int yAxisOptions(int x, int options[SLICE_VAR_COUNT], char *names, size_t size)
{
    int count = 0;
    names[0] = '\0';
    for (int v = 0; v < SLICE_VAR_COUNT; v++)
    {
        if (!yAllowed(x, v))
        {
            continue;
        }
        if (count > 0)
        {
            strncat(names, ";", size - strlen(names) - 1);
        }
        strncat(names, varInfo[v].name, size - strlen(names) - 1);
        options[count++] = v;
    }
    return count;
}

static void drawAxisPickers(SliceViewT *slice)
{
    DrawText("X axis", CONTROL_LABEL_X, xAxisPicker.y + 2, 16, LIGHTGRAY);
    GuiComboBox(xAxisPicker, xAxisNames(), &slice->xAxis);

    int options[SLICE_VAR_COUNT];
    char names[128];
    int count = yAxisOptions(slice->xAxis, options, names, sizeof(names));

    int active = 0;
    for (int i = 0; i < count; i++)
    {
        if (options[i] == slice->yAxis)
        {
            active = i;
        }
    }

    DrawText("Y axis", CONTROL_LABEL_X, yAxisPicker.y + 2, 16, LIGHTGRAY);
    GuiComboBox(yAxisPicker, names, &active);
    slice->yAxis = options[active];
}

static void drawSliders(SliceViewT *slice)
{
    if (!isSVar(slice->xAxis) && !isSVar(slice->yAxis))
    {
        DrawText("S as", CONTROL_LABEL_X, systemPicker.y + 2, 16, LIGHTGRAY);
        GuiToggleGroup(systemPicker, "Re/Im;|S|/arg", &slice->polarSliders);
    }

    DrawText("Follow", CONTROL_LABEL_X, followBox.y + 2, 16, LIGHTGRAY);
    GuiCheckBox(followBox, NULL, &slice->follow);

    if (slice->follow)
    {
        GuiDisable();
    }

    float y = SLIDERS_Y;
    for (int v = 0; v < SLICE_VAR_COUNT; v++)
    {
        if (!isSlider(slice, v))
        {
            continue;
        }
        Rectangle bounds = { SLIDER_X, y, SLIDER_WIDTH, CONTROL_HEIGHT };
        GuiSlider(bounds, TextFormat("%s = %0.2f", varInfo[v].name, slice->values[v]), NULL,
                  &slice->values[v], varInfo[v].min, varInfo[v].max);
        y += SLIDER_SPACING;
    }

    GuiEnable();
}

void model2d_SyncSliceToState(SliceViewT *slice, const State2DT *state)
{
    slice->values[SLICE_RE_S]  = crealf(state->S);
    slice->values[SLICE_IM_S]  = cimagf(state->S);
    slice->values[SLICE_ABS_S] = cabsf(state->S);
    slice->values[SLICE_ARG_S] = cargf(state->S);
    slice->values[SLICE_M]     = state->m;
    slice->values[SLICE_THETA] = state->theta;
}

void model2d_InitSliceView(SliceViewT *slice)
{
    slice->xAxis = SLICE_RE_S;
    slice->yAxis = SLICE_IM_S;
    slice->field = SLICE_F_S;
    slice->polarSliders = 1;
    slice->follow = false;
    for (int v = 0; v < SLICE_VAR_COUNT; v++)
    {
        slice->values[v] = 0.0f;
    }
    slice->values[SLICE_ABS_S] = 0.5f;
}

void model2d_DrawSliceView(void *data)
{
    Model2DDataT *d = model2d_of(data);
    SliceViewT *slice = &d->slice;

    PlotAxesT axes = {
        fieldTitles[slice->field],
        varInfo[slice->xAxis].name, varInfo[slice->yAxis].name,
        varInfo[slice->xAxis].min, varInfo[slice->xAxis].max,
        varInfo[slice->yAxis].min, varInfo[slice->yAxis].max,
        false,
    };

    if (slice->field == SLICE_F_S)
    {
        DrawComplexMap(sliceMap, 6, 2, sliceFS, d, axes, 1.0f);
        DrawColorBar(sliceColorBar, 1, ColorAngle, "arg F_S", -PI, PI);
    }
    else
    {
        DrawMap(sliceMap, 6, 2, 0, sliceScalar, d, axes, FIELD_MIN, FIELD_MAX);
        DrawColorBar(sliceColorBar, 1, fieldColor, fieldNames[slice->field], FIELD_MIN, FIELD_MAX);
    }

    if (slice->follow)
    {
        DrawDot(sliceMap, 2, slice->values[slice->xAxis], slice->values[slice->yAxis], axes, RAYWHITE);
    }

    DrawText("Field", CONTROL_LABEL_X, fieldPicker.y + 2, 16, LIGHTGRAY);
    GuiComboBox(fieldPicker, "F_S;F_M;F_X", &slice->field);

    drawSliders(slice);
    drawAxisPickers(slice);
}
