#ifndef __MODEL_2D_SLICEVIEW_H
#define __MODEL_2D_SLICEVIEW_H

#include "models/model_2d/model.h"

void model2d_InitSliceView(SliceViewT *slice);
void model2d_SyncSliceToState(SliceViewT *slice, const State2DT *state);
void model2d_DrawSliceView(void *data);

#endif
