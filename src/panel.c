#include <assert.h>
#include <string.h>

#include "raygui.h"
#include "panel.h"

void addPanel(PanelListT *list, PanelT panel)
{
  assert(list->count < MAX_PANELS);
  assert(panel.draw != NULL);

  list->panels[list->count++] = panel;
}

void drawActivePanel(PanelListT *list)
{
  if (list->active < 0 || list->active >= list->count)
  {
    return;
  }

  PanelT *panel = &list->panels[list->active];
  panel->draw(panel->ctx);
}

void updateActivePanel(PanelListT *list)
{
  if (list->active < 0 || list->active >= list->count)
  {
    return;
  }

  PanelT *panel = &list->panels[list->active];
  
  if (panel->update)
  {
    panel->update(panel->ctx);
  }
}

void drawPanelMenu(PanelListT *list, Rectangle itemBounds)
{
  // GuiToggleGroup wants the labels as one string separated by ';'
  char labels[512] = "";
  for (int i = 0; i < list->count; i++)
  {
    if (i > 0)
    {
      strncat(labels, ";", sizeof(labels) - strlen(labels) - 1);
    }
    strncat(labels, list->panels[i].name, sizeof(labels) - strlen(labels) - 1);
  }

  GuiToggleGroup(itemBounds, labels, &list->active);
}
