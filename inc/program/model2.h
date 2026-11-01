#ifndef __PROGRAM_MODEL2_H
#define __PROGRAM_MODEL2_H

void initModel2(void);

float pixel_F_S(float S, float m);

float pixel_F_X(float S, float m);

float pixel_F_M(float S, float m);

float pixel_dMdt(StateT *state_p, float dx);

#endif
