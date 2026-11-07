#ifndef __VALUEROW_H
#define __VALUEROW_H

#include <stdbool.h>

typedef struct ValueBoxS {
    char text[33];
    bool editing;
} ValueBoxT;

typedef struct ValueRowLayoutS {
    float labelX;
    float sliderX;
    float sliderWidth;
    float boxWidth;
    float y;
    float spacing;
    float height;
} ValueRowLayoutT;

void DrawValueRow(const ValueRowLayoutT *layout, int row, ValueBoxT *box,
                  const char *label, float *value, float min, float max);
bool AnyValueBoxEditing(const ValueBoxT *boxes, int count);

#endif
