#include <stdlib.h>
#include <raylib.h>

#include "lib/budget.h"
#include "models/registry.h"
#include "models/model_1d/model.h"
#include "models/model_1d/solver.h"
#include "models/model_1d/simview.h"
#include "models/model_1d/editview.h"
#include "models/model_1d/velocity_constant.h"
#include "models/model_1d/velocity_gaussian.h"
#include "models/model_1d/velocity_pixel.h"

static void *createWith(const VelocityFieldT *velocity, const ParametersT *runtime)
{
    Model1DDataT *data = calloc(1, sizeof(Model1DDataT));
    if (data == NULL)
    {
        return NULL;
    }

    data->initM = calloc(MEMORY_GRID_POINTS, sizeof(float));
    allocState(data);
    if (data->initM == NULL || data->state.M == NULL || data->state.Mnext == NULL)
    {
        freeState(data);
        free(data->initM);
        free(data);
        return NULL;
    }

    data->velocity = velocity;
    data->runtime = runtime;
    data->parameters.dt = 0.001f;
    data->parameters.sigma_M = 0.01f;
    data->brushSize = -3.0f;

    velocity->init(data);
    resetState(data);

    return data;
}

static void *createConstant(const ParametersT *runtime)
{
    return createWith(&VelocityConstant, runtime);
}

static void *createGaussian(const ParametersT *runtime)
{
    return createWith(&VelocityGaussian, runtime);
}

static void *createPixel(const ParametersT *runtime)
{
    return createWith(&VelocityPixel, runtime);
}

static void destroy(void *data)
{
    Model1DDataT *d = model1d_of(data);
    freeState(d);
    free(d->initM);
    free(d);
}

static void stepModel(void *data)
{
    stepState(model1d_of(data));
}

static void updateSimulationPanel(void *data)
{
    Model1DDataT *d = model1d_of(data);

    if (IsKeyPressed(KEY_R))
    {
        resetState(d);
    }

    runWithinBudget(d->runtime, stepModel, d);
}

static void addPanels(void *data, PanelListT *panels)
{
    addPanel(panels, (PanelT){
        .name = "Simulation",
        .update = updateSimulationPanel,
        .draw = DrawSimulationView,
        .ctx = data,
    });
    addPanel(panels, (PanelT){
        .name = "Edit",
        .update = HandleInputEditView,
        .draw = DrawEditView,
        .ctx = data,
    });
}

const ModelT Model1DConstant = {
    .name = "1D Constant",
    .create = createConstant,
    .destroy = destroy,
    .addPanels = addPanels,
};

const ModelT Model1DGaussian = {
    .name = "1D Gaussian",
    .create = createGaussian,
    .destroy = destroy,
    .addPanels = addPanels,
};

const ModelT Model1DPixel = {
    .name = "1D Pixel",
    .create = createPixel,
    .destroy = destroy,
    .addPanels = addPanels,
};
