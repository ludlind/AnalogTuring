#ifndef __MODEL_2D_SOLVER_H
#define __MODEL_2D_SOLVER_H

#include "models/model_2d/model.h"

float model2d_periodicMemoryDistance(float a, float b);
float model2d_getMemoryStartTime(const Model2DDataT *data);
float model2d_interpM(const Model2DDataT *data, float x);
float complex model2d_interpS(const Model2DDataT *data, float t);
float model2d_interpTheta(const Model2DDataT *data, float t);
float model2d_interpX(const Model2DDataT *data, float t);
float model2d_interpm(const Model2DDataT *data, float t);
void model2d_allocState(Model2DDataT *data);
void model2d_resetState(Model2DDataT *data);
void model2d_stepState(Model2DDataT *data);
void model2d_freeState(Model2DDataT *data);

#endif
