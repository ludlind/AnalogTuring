#ifndef __MODELS_MODEL1_H
#define __MODELS_MODEL1_H

void initModel1(void);

float gaussian_states_F_S(float S, float m);

float gaussian_states_F_X(float S, float m);

float gaussian_states_F_M(float S, float m);

float gaussian_states_dMdt(StateT *state_p, float dx);

#endif
