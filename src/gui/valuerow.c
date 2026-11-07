#include <stdio.h>
#include <raylib.h>

#include "gui/raygui.h"
#include "gui/valuerow.h"

void DrawValueRow(const ValueRowLayoutT *layout, int row, ValueBoxT *box,
                  const char *label, float *value, float min, float max)
{
    float y = layout->y + row*layout->spacing;
    Rectangle slider = { layout->sliderX, y, layout->sliderWidth, layout->height };
    Rectangle bounds = { layout->sliderX + layout->sliderWidth + 10, y, layout->boxWidth, layout->height };

    DrawText(label, layout->labelX, y + 2, 16, LIGHTGRAY);
    GuiSlider(slider, NULL, NULL, value, min, max);

    if (!box->editing)
    {
        snprintf(box->text, sizeof(box->text), "%.3f", *value);
    }
    if (GuiValueBoxFloat(bounds, NULL, box->text, value, box->editing))
    {
        box->editing = !box->editing;
    }
}

bool AnyValueBoxEditing(const ValueBoxT *boxes, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (boxes[i].editing)
        {
            return true;
        }
    }
    return false;
}
