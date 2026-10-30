#ifndef __MODELS_MODEL0_H
#define __MODELS_MODEL0_H

void initModel0(void);
float F_S(float S, float m);
float F_X(float S, float m);
float F_M(float S, float m);
float dMdt(StateT *state_p, float dx);

#endif
