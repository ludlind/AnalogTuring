#ifndef __PANEL_H
#define __PANEL_H

#include <stdbool.h>
#include <raylib.h>

#define MAX_PANELS 16

typedef void (*PanelFunction)(void *ctx);

typedef struct PanelS {
  const char *name;
  PanelFunction draw;
  PanelFunction update;
  void *ctx;
} PanelT;

typedef struct PanelListS {
  PanelT panels[MAX_PANELS];
  int count;
  int active;
} PanelListT;

void addPanel(PanelListT *list, PanelT panel);
void drawActivePanel(PanelListT *list);
void updateActivePanel(PanelListT *list);
void drawPanelMenu(PanelListT *list, Rectangle itemBounds);

#endif
