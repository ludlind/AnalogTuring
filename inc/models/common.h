#ifndef __MODELS_COMMON_H
#define __MODELS_COMMON_H

#define NUMBER_OF_COLUMNS 20
#define NUMBER_OF_ROWS    20
#define NUMBER_OF_STATES  10

#define SIGMA_FS 0.1f
#define SIGMA_FX 0.1f
#define SIGMA_FM 0.1f

extern int stateMap[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];

extern float FSvalues[NUMBER_OF_STATES];
extern float FXvalues[NUMBER_OF_STATES];
extern float FMvalues[NUMBER_OF_STATES];

extern float* initM;

float GetFSValue(int state);
void SetFSValue(int state, float value);

float GetFXValue(int state);
void SetFXValue(int state, float value);

float GetFMValue(int state);
void SetFMValue(int state, float value);

void StateMapSetValue(float S, float m, int state);
int StateMapGetValue(float S, float m);
float StateMapGetValueF(float S, float m);

void initState(StateT* state_p);

void destroyProgram(void);
 
float interpInitM(float x);

void writeGaussianInitM(float X, float intensity, float sigma);

#endif
