#ifndef __MODEL_2D_KEYFRAME_PROGRAM_H
#define __MODEL_2D_KEYFRAME_PROGRAM_H

#include "models/model_2d/model.h"

void model2d_InitKeyframeProgram(KeyframeProgramT *program);
float model2d_KeyframeTheta(const KeyframeProgramT *program, int keyframe);
float model2d_SymbolLevel(const KeyframeProgramT *program, int symbol);
float complex model2d_EvalSplats(const SplatListT *list, float complex S);
int model2d_AddSplat(SplatListT *list, SplatT splat);
void model2d_RemoveSplat(SplatListT *list, int k);

#endif
