#ifndef __VISUAL_H
#define __VISUAL_H

#include <raylib.h>
#include "common.h"

Color ColorMap(float v);
void DrawMap(Rectangle rect, int cellSize,
             int lineThickness, int colorMap,
             float (*map)(float, float),
             float xmin, float xmax,
             float ymin, float ymax,
             float zmin, float zmax);
void DrawScope(Rectangle rect,
               int lineThickness,
               float (*func)(float),
               float xmin, float xmax,
               float ymin, float ymax);
void DrawDot(Rectangle map, int margin,
             float x, float y,
             float xmin, float xmax,
             float ymin, float ymax,
             Color color);

#endif
