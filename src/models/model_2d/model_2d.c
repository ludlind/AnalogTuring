#include <stdlib.h>
#include <raylib.h>

#include "lib/util.h"
#include "lib/budget.h"
#include "models/registry.h"
#include "models/model_2d/model.h"
#include "models/model_2d/solver.h"
#include "models/model_2d/simview.h"
#include "models/model_2d/velocity_constant.h"

static void *createWith(const VelocityField2DT *velocity, const ParametersT *runtime)
{
    Model2DDataT *data = calloc(1, sizeof(Model2DDataT));
    if (data == NULL)
    {
        return NULL;
    }

    data->initM = calloc(MEMORY_GRID_POINTS, sizeof(float));
    model2d_allocState(data);
    if (data->initM == NULL || data->state.M == NULL || data->state.Mnext == NULL)
    {
        model2d_freeState(data);
        free(data->initM);
        free(data);
        return NULL;
    }

    data->velocity = velocity;
    data->runtime = runtime;
    data->parameters.dt = 0.001f;
    data->parameters.sigma_M = 0.01f;
    data->parameters.omega = 2.0f*PI;

    velocity->init(data);
    model2d_resetState(data);

    return data;
}

static void *createConstant(const ParametersT *runtime)
{
    return createWith(&model2d_VelocityConstant, runtime);
}

static void destroy(void *data)
{
    Model2DDataT *d = model2d_of(data);
    model2d_freeState(d);
    free(d->initM);
    free(d);
}

static void stepModel(void *data)
{
    model2d_stepState(model2d_of(data));
}

static void updateSimulationPanel(void *data)
{
    Model2DDataT *d = model2d_of(data);

    if (IsKeyPressed(KEY_R))
    {
        model2d_resetState(d);
    }

    runWithinBudget(d->runtime, stepModel, d);
}

static void addPanels(void *data, PanelListT *panels)
{
    addPanel(panels, (PanelT){
        .name = "Simulation",
        .update = updateSimulationPanel,
        .draw = model2d_DrawSimulationView,
        .ctx = data,
    });
}

const ModelT Model2DConstant = {
    .name = "2D Constant",
    .create = createConstant,
    .destroy = destroy,
    .addPanels = addPanels,
};
