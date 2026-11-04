#ifndef __MODEL_1D_SOLVER_H
#define __MODEL_1D_SOLVER_H

#include "models/model_1d/model.h"

float periodicMemoryDistance(float a, float b);
float getMemoryStartTime(const Model1DDataT *data);
float interpM(const Model1DDataT *data, float x);
float interpS(const Model1DDataT *data, float t);
float interpX(const Model1DDataT *data, float t);
float interpm(const Model1DDataT *data, float t);
void allocState(Model1DDataT *data);
void resetState(Model1DDataT *data);
void stepState(Model1DDataT *data);
void freeState(Model1DDataT *data);

#endif
