#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "common.h"
#include "gui/visual.h"

#define NUM_OF_COLORMAPS 1

#define AXIS_FONT_SIZE  16
#define TITLE_FONT_SIZE 20
#define AXIS_COLOR      LIGHTGRAY

static void drawRotatedText(const char *text, float x, float y)
{
    Font font = GetFontDefault();
    DrawTextPro(font, text, (Vector2){ x, y }, (Vector2){ 0, 0 }, -90.0f,
                AXIS_FONT_SIZE, AXIS_FONT_SIZE/10.0f, AXIS_COLOR);
}

static void drawAxes(Rectangle rect, PlotAxesT axes)
{
    if (axes.title)
    {
        int w = MeasureText(axes.title, TITLE_FONT_SIZE);
        DrawText(axes.title, rect.x + (rect.width - w)/2, rect.y - TITLE_FONT_SIZE - 4,
                 TITLE_FONT_SIZE, RAYWHITE);
    }

    float below = rect.y + rect.height + 3;
    const char *xminText = TextFormat("%.3g", axes.xmin);
    DrawText(xminText, rect.x, below, AXIS_FONT_SIZE, AXIS_COLOR);
    const char *xmaxText = TextFormat("%.3g", axes.xmax);
    DrawText(xmaxText, rect.x + rect.width - MeasureText(xmaxText, AXIS_FONT_SIZE), below,
             AXIS_FONT_SIZE, AXIS_COLOR);
    if (axes.xlabel)
    {
        int w = MeasureText(axes.xlabel, AXIS_FONT_SIZE);
        DrawText(axes.xlabel, rect.x + (rect.width - w)/2, below, AXIS_FONT_SIZE, AXIS_COLOR);
    }

    const char *ymaxText = TextFormat("%.3g", axes.ymax);
    DrawText(ymaxText, rect.x - MeasureText(ymaxText, AXIS_FONT_SIZE) - 4, rect.y,
             AXIS_FONT_SIZE, AXIS_COLOR);
    const char *yminText = TextFormat("%.3g", axes.ymin);
    DrawText(yminText, rect.x - MeasureText(yminText, AXIS_FONT_SIZE) - 4,
             rect.y + rect.height - AXIS_FONT_SIZE, AXIS_FONT_SIZE, AXIS_COLOR);
    if (axes.ylabel)
    {
        int w = MeasureText(axes.ylabel, AXIS_FONT_SIZE);
        drawRotatedText(axes.ylabel, rect.x - 4 - AXIS_FONT_SIZE, rect.y + (rect.height + w)/2);
    }
}


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

Color ColorMap(float v)
{
    return ColorMap1(clamp(v, 0.0f, 1.0f));
}

void DrawMap(Rectangle rect, int cellSize,
             int lineThickness, int colorMap,
             MapFunc map, const void *ctx,
             PlotAxesT axes,
             float zmin, float zmax)
{
    float xmin = axes.xmin, xmax = axes.xmax;
    float ymin = axes.ymin, ymax = axes.ymax;

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

            float v = (map(ctx, x, y) - zmin)/(zmax - zmin);
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

    drawAxes(rect, axes);
}

static Rectangle innerRect(Rectangle rect, int lineThickness)
{
    return (Rectangle){rect.x + lineThickness,
                       rect.y + lineThickness,
                       rect.width - 2*lineThickness,
                       rect.height - 2*lineThickness};
}

static void drawScope(Rectangle rect,
                      int lineThickness,
                      ScopeFunc func, ScopeColorFunc colorFunc,
                      const void *ctx,
                      PlotAxesT axes)
{
    float xmin = axes.xmin, xmax = axes.xmax;
    float ymin = axes.ymin, ymax = axes.ymax;

    DrawRectangleRec(rect, (Color){ 15, 18, 24, 255 });
    DrawRectangleLinesEx(rect, lineThickness, RAYWHITE);

    Rectangle inner = innerRect(rect, lineThickness);

    // Grid
    for (int i = 1; i < 4; i++) {
        float y = inner.y + inner.height * i / 4.0f;
        DrawLine(inner.x, y, inner.x + inner.width, y, (Color){ 60, 70, 80, 255 });
    }

    for (int i = 1; i < 8 && !axes.hideXGrid; i++) {
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

        float v = (func(ctx, xarg) - ymin)/(ymax - ymin);
        v = clamp(v, 0.0f, 1.0f);

        Vector2 p = {
            inner.x + i,
            inner.y + inner.height - v * inner.height
        };

        if (i > 0) {
            Color color = colorFunc ? colorFunc(ctx, xarg) : GREEN;
            DrawLineV(prev, p, color);
        }

        prev = p;
    }

    drawAxes(rect, axes);
}

void DrawScope(Rectangle rect,
               int lineThickness,
               ScopeFunc func, const void *ctx,
               PlotAxesT axes)
{
    drawScope(rect, lineThickness, func, NULL, ctx, axes);
}

void DrawColoredScope(Rectangle rect,
                      int lineThickness,
                      ScopeFunc func, ScopeColorFunc colorFunc,
                      const void *ctx,
                      PlotAxesT axes)
{
    drawScope(rect, lineThickness, func, colorFunc, ctx, axes);
}

void DrawVerticalLine(Rectangle rect, int lineThickness,
                      float x,
                      PlotAxesT axes,
                      Color color)
{
    if (x < axes.xmin || x > axes.xmax || axes.xmax <= axes.xmin)
    {
        return;
    }

    Rectangle inner = innerRect(rect, lineThickness);
    float px = inner.x + (x - axes.xmin)/(axes.xmax - axes.xmin)*inner.width;
    DrawLine(px, inner.y, px, inner.y + inner.height, color);
}

void DrawColorBar(Rectangle rect, int lineThickness,
                  ColorFunc color,
                  const char *label,
                  float vmin, float vmax)
{
    Rectangle inner = innerRect(rect, lineThickness);
    for (int py = 0; py < inner.height; py++)
    {
        float v = vmax - ((float)py / inner.height) * (vmax - vmin);
        DrawLine(inner.x, inner.y + py, inner.x + inner.width, inner.y + py, color(v));
    }
    DrawRectangleLinesEx(rect, lineThickness, RAYWHITE);

    float right = rect.x + rect.width + 4;
    DrawText(TextFormat("%.3g", vmax), right, rect.y, AXIS_FONT_SIZE, AXIS_COLOR);
    DrawText(TextFormat("%.3g", 0.5f*(vmin + vmax)), right,
             rect.y + (rect.height - AXIS_FONT_SIZE)/2, AXIS_FONT_SIZE, AXIS_COLOR);
    DrawText(TextFormat("%.3g", vmin), right,
             rect.y + rect.height - AXIS_FONT_SIZE, AXIS_FONT_SIZE, AXIS_COLOR);

    if (label)
    {
        int w = MeasureText(label, AXIS_FONT_SIZE);
        DrawText(label, rect.x + (rect.width - w)/2, rect.y - AXIS_FONT_SIZE - 4,
                 AXIS_FONT_SIZE, RAYWHITE);
    }
}

Color ColorAngle(float angle)
{
    float hue = (angle + PI)/(2.0f*PI)*360.0f;
    return ColorFromHSV(hue, 0.8f, 1.0f);
}

static Vector2 polarCenter(Rectangle rect)
{
    return (Vector2){ rect.x + rect.width/2.0f, rect.y + rect.height/2.0f };
}

static float polarPixelRadius(Rectangle rect)
{
    return fminf(rect.width, rect.height)/2.0f;
}

static Vector2 polarToScreen(Rectangle rect, float radius, float complex z)
{
    Vector2 c = polarCenter(rect);
    float R = polarPixelRadius(rect);
    return (Vector2){ c.x + crealf(z)/radius*R, c.y - cimagf(z)/radius*R };
}

static void drawCenteredText(const char *text, Vector2 p, int fontSize, Color color)
{
    int w = MeasureText(text, fontSize);
    DrawText(text, p.x - w/2.0f, p.y - fontSize/2.0f, fontSize, color);
}

static void drawPolarGuides(Rectangle rect, float radius)
{
    Vector2 c = polarCenter(rect);
    float R = polarPixelRadius(rect);
    Color guide = { 255, 255, 255, 60 };

    for (int i = 1; i < 4; i++)
    {
        DrawCircleLinesV(c, R*i/4.0f, guide);
    }
    for (int i = 0; i < 8; i++)
    {
        float a = i*PI/4.0f;
        DrawLineV(c, (Vector2){ c.x + R*cosf(a), c.y - R*sinf(a) }, guide);
    }
    DrawRing(c, R, R + 2, 0.0f, 360.0f, 128, RAYWHITE);

    static const char *angleLabels[] = { "pi/2", "pi", "-pi/2" };
    float labelR = R + 4 + AXIS_FONT_SIZE/2.0f;
    for (int i = 0; i < 3; i++)
    {
        float a = (i + 1)*PI/2.0f;
        Vector2 p = { c.x + labelR*cosf(a), c.y - labelR*sinf(a) };
        if (i == 1) p.x -= MeasureText(angleLabels[i], AXIS_FONT_SIZE)/2.0f;
        drawCenteredText(angleLabels[i], p, AXIS_FONT_SIZE, AXIS_COLOR);
    }

    DrawText(TextFormat("%.3g", 0.5f*radius), c.x + R/2.0f + 2, c.y + 2, AXIS_FONT_SIZE, AXIS_COLOR);
    DrawText(TextFormat("%.3g", radius), c.x + R - MeasureText(TextFormat("%.3g", radius), AXIS_FONT_SIZE) - 2,
             c.y + 2, AXIS_FONT_SIZE, AXIS_COLOR);
}

typedef Color (*PolarCellColorFunc)(const void *cellCtx, float complex z);

typedef struct ComplexCellS {
    ComplexMapFunc map;
    const void *ctx;
    float scale;
} ComplexCellT;

typedef struct ScalarCellS {
    PolarScalarFunc map;
    const void *ctx;
    float zmin, zmax;
} ScalarCellT;

static Color complexCellColor(const void *cellCtx, float complex z)
{
    const ComplexCellT *cell = cellCtx;
    float complex F = cell->map(cell->ctx, z);

    Color color = ColorAngle(cargf(F));
    float brightness = 1.0f - expf(-cabsf(F)/cell->scale);
    return ColorBrightness(color, brightness - 1.0f);
}

static Color scalarCellColor(const void *cellCtx, float complex z)
{
    const ScalarCellT *cell = cellCtx;
    float v = cell->map(cell->ctx, z);
    return ColorMap((v - cell->zmin)/(cell->zmax - cell->zmin));
}

static void drawPolar(Rectangle rect, int rings, int sectors,
                      PolarCellColorFunc cellColor, const void *cellCtx,
                      const char *title, float radius)
{
    Vector2 c = polarCenter(rect);
    float R = polarPixelRadius(rect);
    float sectorDeg = 360.0f/sectors;

    for (int i = 0; i < rings; i++)
    {
        float r0 = R*i/rings;
        float r1 = R*(i + 1)/rings;
        float r = radius*(i + 0.5f)/rings;

        for (int j = 0; j < sectors; j++)
        {
            float a = -PI + (j + 0.5f)*2.0f*PI/sectors;
            Color color = cellColor(cellCtx, r*cexpf(I*a));

            float a0 = -180.0f + j*sectorDeg;
            DrawRing(c, r0, r1, -(a0 + sectorDeg), -a0, 2, color);
        }
    }

    drawPolarGuides(rect, radius);

    if (title)
    {
        int w = MeasureText(title, TITLE_FONT_SIZE);
        DrawText(title, rect.x + (rect.width - w)/2,
                 rect.y - AXIS_FONT_SIZE - 8 - TITLE_FONT_SIZE,
                 TITLE_FONT_SIZE, RAYWHITE);
    }
}

void DrawPolarMap(Rectangle rect, int rings, int sectors,
                  ComplexMapFunc map, const void *ctx,
                  const char *title, float radius, float scale)
{
    ComplexCellT cell = { map, ctx, scale };
    drawPolar(rect, rings, sectors, complexCellColor, &cell, title, radius);
}

void DrawPolarScalarMap(Rectangle rect, int rings, int sectors,
                        PolarScalarFunc map, const void *ctx,
                        const char *title, float radius,
                        float zmin, float zmax)
{
    ScalarCellT cell = { map, ctx, zmin, zmax };
    drawPolar(rect, rings, sectors, scalarCellColor, &cell, title, radius);
}

void DrawPolarDot(Rectangle rect, float radius,
                  float complex z, Color color)
{
    if (cabsf(z) > radius)
    {
        return;
    }
    DrawCircleV(polarToScreen(rect, radius, z), 3.0f, color);
}

void DrawDot(Rectangle map, int margin,
             float x, float y,
             PlotAxesT axes,
             Color color)
{
    float xmin = axes.xmin, xmax = axes.xmax;
    float ymin = axes.ymin, ymax = axes.ymax;

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
