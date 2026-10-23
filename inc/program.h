#ifndef __PROGRAM_H
#define __PROGRAM_H

#include "common.h"

#define NUMBER_OF_COLUMNS 20
#define NUMBER_OF_ROWS    20
#define NUMBER_OF_STATES  10

#define SIGMA_FS 0.1f
#define SIGMA_FX 0.1f
#define SIGMA_FM 0.1f

ProgramT *getProgram(void);

float GetFSValue(int state);
float GetFXValue(int state);
float GetFMValue(int state);

void SetFSValue(int state, float value);
void SetFXValue(int state, float value);
void SetFMValue(int state, float value);

void StateMapSetValue(float S, float m, int state);
int StateMapGetValue(float S, float m);
float StateMapGetValueF(float S, float m);
float GetStartS(void);
void SetStartS(float S);
float interpInitM(float x);
float writeGaussianInitM(float X, float intensity, float sigma);

#endif
