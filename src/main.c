#include <stdio.h>
#include <raylib.h>

#include "common.h"
#include "gui/raygui.h"
#include "core/panel.h"
#include "models/registry.h"
#include "paramview.h"

ParametersT parameters = {
    0.3f,
    100,
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

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Continuous Turing Machine");
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    const ModelT *model = getModel(2);
    void *data = model->create(&parameters);
    if (data == NULL)
    {
        fprintf(stderr, "Failed to create model '%s'\n", model->name);
        CloseWindow();
        return 1;
    }

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

    model->addPanels(data, &panels);
    addPanel(&panels, (PanelT){
        .name = "Settings",
        .update = NULL,
        .draw = drawParamPanel,
        .ctx = &parameters,
    });

    while (!WindowShouldClose())
    {
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
