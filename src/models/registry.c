#include <assert.h>

#include "models/registry.h"

extern const ModelT Model1DGaussian;
extern const ModelT Model1DPixel;
extern const ModelT Model2DConstant;

static const ModelT *const models[] = {
    &Model1DGaussian,
    &Model1DPixel,
    &Model2DConstant,
};

int getModelCount(void)
{
    return (int)(sizeof(models) / sizeof(models[0]));
}

const ModelT *getModel(int modelIndex)
{
    assert(modelIndex >= 0 && modelIndex < getModelCount());
    return models[modelIndex];
}
