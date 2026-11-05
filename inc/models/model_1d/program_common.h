#ifndef __PROGRAM_COMMON_H
#define __PROGRAM_COMMON_H

#include "models/model_1d/model.h"

void model1d_StateMapSetValue(Model1DDataT *data, float S, float m, int state);
int model1d_StateMapGetValue(const Model1DDataT *data, float S, float m);
float model1d_StateMapGetValueF(const Model1DDataT *data, float S, float m);

float model1d_interpInitM(const Model1DDataT *data, float x);
void model1d_writeGaussianInitM(Model1DDataT *data, float X, float intensity, float sigma);

#endif
