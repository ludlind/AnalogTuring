#ifndef __VISUAL_H
#define __VISUAL_H

#include <raylib.h>
#include "common.h"

typedef float (*MapFunc)(const void *ctx, float x, float y);
typedef float (*ScopeFunc)(const void *ctx, float x);

Color ColorMap(float v);
void DrawMap(Rectangle rect, int cellSize,
             int lineThickness, int colorMap,
             MapFunc map, const void *ctx,
             float xmin, float xmax,
             float ymin, float ymax,
             float zmin, float zmax);
void DrawScope(Rectangle rect,
               int lineThickness,
               ScopeFunc func, const void *ctx,
               float xmin, float xmax,
               float ymin, float ymax);
void DrawDot(Rectangle map, int margin,
             float x, float y,
             float xmin, float xmax,
             float ymin, float ymax,
             Color color);

#endif
