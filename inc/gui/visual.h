#ifndef __VISUAL_H
#define __VISUAL_H

#include <complex.h>
#include <raylib.h>
#include "common.h"

typedef float (*MapFunc)(const void *ctx, float x, float y);
typedef float complex (*ComplexMapFunc)(const void *ctx, float complex z);
typedef float (*PolarScalarFunc)(const void *ctx, float complex z);
typedef float (*ScopeFunc)(const void *ctx, float x);
typedef Color (*ScopeColorFunc)(const void *ctx, float x);
typedef Color (*ColorFunc)(float v);

typedef struct PlotAxesS {
    const char *title;
    const char *xlabel;
    const char *ylabel;
    float xmin, xmax;
    float ymin, ymax;
    bool hideXGrid;
} PlotAxesT;

Color ColorMap(float v);
Color ColorAngle(float angle);
void DrawPolarMap(Rectangle rect, int rings, int sectors,
                  ComplexMapFunc map, const void *ctx,
                  const char *title, float radius, float scale);
void DrawPolarScalarMap(Rectangle rect, int rings, int sectors,
                        PolarScalarFunc map, const void *ctx,
                        const char *title, float radius,
                        float zmin, float zmax);
void DrawPolarDot(Rectangle rect, float radius,
                  float complex z, Color color);
void DrawMap(Rectangle rect, int cellSize,
             int lineThickness, int colorMap,
             MapFunc map, const void *ctx,
             PlotAxesT axes,
             float zmin, float zmax);
void DrawScope(Rectangle rect,
               int lineThickness,
               ScopeFunc func, const void *ctx,
               PlotAxesT axes);
void DrawColoredScope(Rectangle rect,
                      int lineThickness,
                      ScopeFunc func, ScopeColorFunc colorFunc,
                      const void *ctx,
                      PlotAxesT axes);
void DrawVerticalLine(Rectangle rect, int lineThickness,
                      float x,
                      PlotAxesT axes,
                      Color color);
void DrawColorBar(Rectangle rect, int lineThickness,
                  ColorFunc color,
                  const char *label,
                  float vmin, float vmax);
void DrawDot(Rectangle map, int margin,
             float x, float y,
             PlotAxesT axes,
             Color color);

#endif
