#ifndef __PROGRAM_COMMON_H
#define __PROGRAM_COMMON_H

#include "models/model_1d/model.h"

void StateMapSetValue(Model1DDataT *data, float S, float m, int state);
int StateMapGetValue(const Model1DDataT *data, float S, float m);
float StateMapGetValueF(const Model1DDataT *data, float S, float m);

float interpInitM(const Model1DDataT *data, float x);
void writeGaussianInitM(Model1DDataT *data, float X, float intensity, float sigma);

#endif
