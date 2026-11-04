#include <raylib.h>

#include "gui/raygui.h"
#include "paramview.h"

static Rectangle maxStepsSlider = { 360, 120, 300, 20 };
static Rectangle simBudgetSlider = { 360, 150, 300, 20 };

void DrawParamView(ParametersT* parameters_p)
{
    float maxStepsVar = parameters_p->maxSteps/1000.0f;
    GuiSlider(maxStepsSlider, TextFormat("Maximum Steps = %d", parameters_p->maxSteps),
              NULL, &maxStepsVar, 0.001f, 1.0f);
    parameters_p->maxSteps = (int)(maxStepsVar*1000);

    GuiSlider(simBudgetSlider, TextFormat("Simulation Budget = %0.2f%%", parameters_p->simBudget*100),
              NULL, &parameters_p->simBudget, 0.0f, 1.0f);
}
