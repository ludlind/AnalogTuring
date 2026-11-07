#ifndef __MODEL_2D_TAPE_H
#define __MODEL_2D_TAPE_H

#include "models/model_2d/model.h"

float model2d_InterpInitM(const Model2DDataT *data, float x);
void model2d_WriteGaussianInitM(Model2DDataT *data, float X, float intensity, float sigma);

#endif
