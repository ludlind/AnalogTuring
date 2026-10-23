#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "common.h"
#include "visual.h"

#define NUM_OF_COLORMAPS 1


Color ColorMap1(float v)
{
    // v is expected in [-1, 1]
    float n = (v + 1.0f) * 0.5f;

    unsigned char r = (unsigned char)(255.0f * v);
    unsigned char g = (unsigned char)(120.0f * (1.0f - fabsf(2.0f*v -1.0f)));
    unsigned char b = (unsigned char)(255.0f * (1.0f - v));

    return (Color){ r, g, b, 255 };
}

Color (*ColorMaps[NUM_OF_COLORMAPS])(float) = {
    ColorMap1,
};

void DrawMap(Rectangle rect, int cellSize,
             int lineThickness, int colorMap,
             float (*map)(float, float),
             float xmin, float xmax,
             float ymin, float ymax,
             float zmin, float zmax)
{
    assert(colorMap >= 0 && colorMap < NUM_OF_COLORMAPS);
    Color (*ColorMap)(float) = ColorMaps[colorMap];

    DrawRectangleLinesEx(rect, lineThickness, RAYWHITE);
    Rectangle inner = (Rectangle){rect.x + lineThickness,
                                  rect.y + lineThickness,
                                  rect.width - 2*lineThickness,
                                  rect.height - 2*lineThickness};
    for (int py = 0; py < inner.height; py += cellSize) {
        for (int px = 0; px < inner.width; px += cellSize) {
            float x = xmin + ((float)px / inner.width) * (xmax - xmin);
            float y = ymin + (1.0f - ((float)py / inner.height)) * (ymax - ymin);

            float v = (map(x, y) - zmin)/(zmax - zmin);
            v = clamp(v, 0.0f, 1.0f);

            DrawRectangle(
                (int)inner.x + px,
                (int)inner.y + py,
                fmin(cellSize, inner.width - px),
                fmin(cellSize, inner.height - py),
                ColorMap(v)
            );
        }
    }
}

void DrawScope(Rectangle rect,
               int lineThickness,
               float (*func)(float),
               float xmin, float xmax,
               float ymin, float ymax)
{
    DrawRectangleRec(rect, (Color){ 15, 18, 24, 255 });
    DrawRectangleLinesEx(rect, lineThickness, RAYWHITE);

    Rectangle inner = (Rectangle){rect.x + lineThickness,
                                  rect.y + lineThickness,
                                  rect.width - 2*lineThickness,
                                  rect.height - 2*lineThickness};

    // Grid
    for (int i = 1; i < 4; i++) {
        float y = inner.y + inner.height * i / 4.0f;
        DrawLine(inner.x, y, inner.x + inner.width, y, (Color){ 60, 70, 80, 255 });
    }

    for (int i = 1; i < 8; i++) {
        float x = inner.x + inner.width * i / 8.0f;
        DrawLine(x, inner.y, x, inner.y + inner.height, (Color){ 60, 70, 80, 255 });
    }

    // Center axis
    float cy = inner.y + inner.height * 0.5f;
    DrawLine(inner.x, cy, inner.x + inner.width, cy, GRAY);

    // Waveform
    Vector2 prev = { 0 };

    for (int i = 0; i < inner.width; i++) {
        float u = (float)i / inner.width;
        float xarg = xmin + u * (xmax - xmin);

        float v = (func(xarg) - ymin)/(ymax - ymin);
        v = clamp(v, 0.0f, 1.0f);

        Vector2 p = {
            inner.x + i,
            inner.y + inner.height - v * inner.height
        };

        if (i > 0) {
            DrawLineV(prev, p, GREEN);
        }

        prev = p;
    }
}

void DrawDot(Rectangle map, int margin,
             float x, float y,
             float xmin, float xmax,
             float ymin, float ymax,
             Color color)
{
    if(x > xmax || x < xmin || y > ymax || y < ymin)
    {
        return;
    }
    x = (x - xmin)/(xmax - xmin);
    y = (y - ymin)/(ymax - ymin);
    DrawCircle((int)(map.x + margin + x*(map.width - 2*margin)),
               (int)(map.y + map.height - margin - y*(map.height - 2*margin)),
               2.0f, color);
}
