#include <string.h>
#include <raylib.h>

#include "gui/raygui.h"
#include "models/registry.h"
#include "paramview.h"

static Rectangle modelPicker = { 360, 90, 300, 20 };
static Rectangle maxStepsSlider = { 360, 120, 300, 20 };
static Rectangle simBudgetSlider = { 360, 150, 300, 20 };

static const char *modelNames(void)
{
    static char names[512] = "";
    if (names[0] != '\0')
    {
        return names;
    }

    for (int i = 0; i < getModelCount(); i++)
    {
        if (i > 0)
        {
            strncat(names, ";", sizeof(names) - strlen(names) - 1);
        }
        strncat(names, getModel(i)->name, sizeof(names) - strlen(names) - 1);
    }
    return names;
}

void DrawParamView(SettingsT *settings)
{
    ParametersT *parameters = &settings->parameters;

    GuiComboBox(modelPicker, modelNames(), &settings->selectedModel);

    float maxStepsVar = parameters->maxSteps/1000.0f;
    GuiSlider(maxStepsSlider, TextFormat("Maximum Steps = %d", parameters->maxSteps),
              NULL, &maxStepsVar, 0.001f, 1.0f);
    parameters->maxSteps = (int)(maxStepsVar*1000);

    GuiSlider(simBudgetSlider, TextFormat("Simulation Budget = %0.2f%%", parameters->simBudget*100),
              NULL, &parameters->simBudget, 0.0f, 1.0f);
}
