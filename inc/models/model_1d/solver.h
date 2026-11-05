#ifndef __MODEL_1D_SOLVER_H
#define __MODEL_1D_SOLVER_H

#include "models/model_1d/model.h"

float model1d_periodicMemoryDistance(float a, float b);
float model1d_getMemoryStartTime(const Model1DDataT *data);
float model1d_interpM(const Model1DDataT *data, float x);
float model1d_interpS(const Model1DDataT *data, float t);
float model1d_interpX(const Model1DDataT *data, float t);
float model1d_interpm(const Model1DDataT *data, float t);
void model1d_allocState(Model1DDataT *data);
void model1d_resetState(Model1DDataT *data);
void model1d_stepState(Model1DDataT *data);
void model1d_freeState(Model1DDataT *data);

#endif
