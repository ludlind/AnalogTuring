#include <stdio.h>
#include <raylib.h>

#include "common.h"
#include "gui/raygui.h"
#include "core/panel.h"
#include "models/registry.h"
#include "paramview.h"

SettingsT settings = {
    .parameters = {
        .simBudget = 0.3f,
        .maxSteps = 100,
    },
    .selectedModel = 2,
};

PanelListT panels;

static Rectangle menuPane = {
    0, 0, WIDTH, 50
};
static Rectangle mainPane = {
    0, 50, WIDTH, HEIGHT - 50
};

static void drawParamPanel(void *ctx)
{
    DrawParamView(ctx);
}

static void buildPanels(const ModelT *model, void *data)
{
    clearPanels(&panels);
    model->addPanels(data, &panels);
    addPanel(&panels, (PanelT){
        .name = "Settings",
        .update = NULL,
        .draw = drawParamPanel,
        .ctx = &settings,
    });
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Continuous Turing Machine");
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    int modelIndex = settings.selectedModel;
    const ModelT *model = getModel(modelIndex);
    void *data = model->create(&settings.parameters);
    if (data == NULL)
    {
        fprintf(stderr, "Failed to create model '%s'\n", model->name);
        CloseWindow();
        return 1;
    }
    buildPanels(model, data);

    Camera2D menuPaneCamera = {0};
    menuPaneCamera.target = (Vector2){0,0};
    menuPaneCamera.offset = (Vector2){menuPane.x, menuPane.y};
    menuPaneCamera.zoom = 1.0f;
    menuPaneCamera.rotation = 0.0f;

    Camera2D mainPaneCamera = {0};
    mainPaneCamera.target = (Vector2){0,0};
    mainPaneCamera.offset = (Vector2){mainPane.x, mainPane.y};
    mainPaneCamera.zoom = 1.0f;
    mainPaneCamera.rotation = 0.0f;

    while (!WindowShouldClose())
    {
        if (settings.selectedModel != modelIndex)
        {
            const ModelT *next = getModel(settings.selectedModel);
            void *nextData = next->create(&settings.parameters);
            if (nextData == NULL)
            {
                fprintf(stderr, "Failed to create model '%s'\n", next->name);
                settings.selectedModel = modelIndex;
            }
            else
            {
                model->destroy(data);
                model = next;
                data = nextData;
                modelIndex = settings.selectedModel;
                buildPanels(model, data);
                panels.active = panels.count - 1;
            }
        }

        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        SetMouseOffset(-mainPane.x, -mainPane.y);
        updateActivePanel(&panels);

        BeginDrawing();
        ClearBackground((Color){ 8, 10, 14, 255 });
        BeginScissorMode(mainPane.x, mainPane.y, mainPane.width, mainPane.height);
        BeginMode2D(mainPaneCamera);
        SetMouseOffset(-mainPane.x, -mainPane.y);
        drawActivePanel(&panels);
        EndMode2D();
        EndScissorMode();
        BeginScissorMode(menuPane.x, menuPane.y, menuPane.width, menuPane.height);
        BeginMode2D(menuPaneCamera);
        SetMouseOffset(-menuPane.x, -menuPane.y);
        drawPanelMenu(&panels, (Rectangle){20, 20, 160, 20});
        EndMode2D();
        EndScissorMode();
        EndDrawing();
    }

    model->destroy(data);

    CloseWindow();
    return 0;
}
