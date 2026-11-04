#ifndef __VISUAL_H
#define __VISUAL_H

#include <raylib.h>
#include "common.h"

typedef float (*MapFunc)(const void *ctx, float x, float y);
typedef float (*ScopeFunc)(const void *ctx, float x);

typedef struct PlotAxesS {
    const char *title;
    const char *xlabel;
    const char *ylabel;
    float xmin, xmax;
    float ymin, ymax;
} PlotAxesT;

Color ColorMap(float v);
void DrawMap(Rectangle rect, int cellSize,
             int lineThickness, int colorMap,
             MapFunc map, const void *ctx,
             PlotAxesT axes,
             float zmin, float zmax);
void DrawScope(Rectangle rect,
               int lineThickness,
               ScopeFunc func, const void *ctx,
               PlotAxesT axes);
void DrawDot(Rectangle map, int margin,
             float x, float y,
             PlotAxesT axes,
             Color color);

#endif
