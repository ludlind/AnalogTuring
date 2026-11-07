#include <math.h>
#include <complex.h>
#include <raylib.h>

#include "gui/raygui.h"
#include "gui/valuerow.h"
#include "lib/util.h"
#include "models/model_2d/solver.h"
#include "models/model_2d/parameterview.h"

static const ValueRowLayoutT layout = {
    .labelX = 80,
    .sliderX = 240,
    .sliderWidth = 300,
    .boxWidth = 80,
    .y = 80,
    .spacing = 35,
    .height = 20,
};

static Rectangle resetButton = { 240, 380, 120, 30 };

void model2d_DrawParameterView(void *data)
{
    Model2DDataT *d = model2d_of(data);
    Model2DParametersT *p = &d->parameters;
    ValueBoxT *boxes = d->parameterView.boxes;

    float logDt = log10f(p->dt);
    float logSigmaM = log10f(p->sigma_M);
    float startRe = crealf(p->startS);
    float startIm = cimagf(p->startS);

    int row = 0;
    DrawValueRow(&layout, row, &boxes[row], "dt (log)", &logDt, -6.0f, -1.0f); row++;
    DrawValueRow(&layout, row, &boxes[row], "sigma_M (log)", &logSigmaM, -4.0f, 0.0f); row++;
    DrawValueRow(&layout, row, &boxes[row], "omega", &p->omega, 0.0f, 4.0f*PI); row++;
    DrawValueRow(&layout, row, &boxes[row], "S_start Re", &startRe, -STATE_MAX_RADIUS, STATE_MAX_RADIUS); row++;
    DrawValueRow(&layout, row, &boxes[row], "S_start Im", &startIm, -STATE_MAX_RADIUS, STATE_MAX_RADIUS); row++;
    DrawValueRow(&layout, row, &boxes[row], "theta_start", &p->startTheta, 0.0f, 2.0f*PI); row++;
    DrawValueRow(&layout, row, &boxes[row], "X_start", &p->startX, MEMORY_START_POSITION, MEMORY_STOP_POSITION); row++;
    DrawValueRow(&layout, row, &boxes[row], "m_start", &p->startm, MEMORY_MIN_VALUE, MEMORY_MAX_VALUE); row++;

    p->dt = powf(10.0f, logDt);
    p->sigma_M = powf(10.0f, logSigmaM);
    p->startS = startRe + I*startIm;

    if (GuiButton(resetButton, "Reset"))
    {
        model2d_resetState(d);
    }
    DrawText("Parameters take effect on reset (button, or R in Simulation, Slice or here)",
             layout.labelX, resetButton.y + 50, 16, GRAY);
}

void model2d_HandleInputParameterView(void *data)
{
    Model2DDataT *d = model2d_of(data);

    if (IsKeyPressed(KEY_R) && !AnyValueBoxEditing(d->parameterView.boxes, PARAMETER_ROWS))
    {
        model2d_resetState(d);
    }
}
