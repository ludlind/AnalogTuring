#ifndef __MODEL_2D_EDITVIEW_H
#define __MODEL_2D_EDITVIEW_H

#include "models/model_2d/model.h"

void model2d_InitKeyframeEdit(KeyframeEditT *edit);
void model2d_DrawEditView(void *data);
void model2d_HandleInputEditView(void *data);

#endif
