#ifndef __STATE_H
#define __STATE_H

#include "common.h"

float periodicMemoryDistance(float a, float b);
float getS(void);
float getX(void);
float getm(void);
float getTime(void);
float getDT(void);
float getMemoryStartTime(void);
float getMemoryStartPosition(void);
float getMemoryStopPosition(void);
float getMemoryMinValue(void);
float getMemoryMaxValue(void);
float getStateMinValue(void);
float getStateMaxValue(void);
float interpM(float x);
float interpS(float t);
float interpX(float t);
float interpm(float t);
void InitState(ParametersT* parameters_p, ProgramT* program_p);
void UpdateState(ProgramT* program_p);
void DestroyState(void);

#endif
