#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "gui/raygui.h"

#include "gui/visual.h"
#include "models/paramview.h"

static Rectangle maxStepsSlider = { 360, 120, 300, 20 };
static Rectangle simBudgetSlider = { 360, 150, 300, 20 };
static Rectangle dtSlider = { 360, 180, 300, 20 };
static Rectangle sigmaMSlider = { 360, 210, 300, 20 };

void DrawParamView(ParametersT* parameters_p)
{
    float maxStepsVar = 0.01f;
    float logDt = log10(parameters_p->dt);
    float logSigmaM = log10(parameters_p->sigma_M);

    GuiSlider(maxStepsSlider, TextFormat("Maximum Steps = %0.0f", maxStepsVar*1000.0f),
              NULL, &maxStepsVar, 0.001f, 1.0f);
    parameters_p->maxSteps = (int)(maxStepsVar*1000);
    maxStepsVar = parameters_p->maxSteps/1000.0f;

    GuiSlider(simBudgetSlider, TextFormat("Simulation Budget = %0.2f\%", parameters_p->simBudget*100),
              NULL, &parameters_p->simBudget, 0.0f, 1.0f);

    GuiSlider(dtSlider, TextFormat("Delta T (log) = %0.2f\%", logDt),
              NULL, &logDt, -10.0f, 10.0f);
    parameters_p->dt = pow(10.0f, logDt);

    GuiSlider(sigmaMSlider, TextFormat("Sigma_M (log) = %0.2f\%", logSigmaM),
              NULL, &logSigmaM, -10.0f, 10.0f);
    parameters_p->sigma_M = pow(10.0f, logSigmaM);
}
