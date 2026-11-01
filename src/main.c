#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "common.h"
#include "gui/raygui.h"
#include "state.h"
#include "program.h"
#include "models/simview.h"
#include "models/editview.h"
#include "models/paramview.h"
#include "core/panel.h"

ParametersT parameters = {
    0.3,
    100,
    0.001f,
    0.01f,
};

AppT app = {
    .cursor = MOUSE_CURSOR_DEFAULT,
    .program_p = NULL,          // set in main() once the program is initialized
    .parameters_p = &parameters,
};

PanelListT panels;

static Rectangle menuPane = {
    0, 0, WIDTH, 50
};
static Rectangle mainPane = {
    0, 50, WIDTH, HEIGHT - 50
};

/*---------PANELS--------*/

static void updateSimulationPanel(void *ctx)
{
    AppT *app_p = ctx;

    // Called at the very start of the frame, so this is effectively the frame start.
    double frameStart = GetTime();

    if (IsKeyPressed(KEY_R))
    {
        InitState(app_p->parameters_p, app_p->program_p);
    }

    int steps = 0;
    while ((GetTime() - frameStart) < app_p->parameters_p->simBudget*GetFrameTime()
           && steps < app_p->parameters_p->maxSteps)
    {
        UpdateState(app_p->program_p);
        steps++;
    }
}

static void drawSimulationPanel(void *ctx)
{
    AppT *app_p = ctx;
    DrawSimulationView(app_p->program_p);
}

static void updateEditPanel(void *ctx)
{
    AppT *app_p = ctx;
    HandleInputEditView(app_p, app_p->program_p);
}

static void drawEditPanel(void *ctx)
{
    AppT *app_p = ctx;
    DrawEditView(app_p->program_p);
}

static void drawParamPanel(void *ctx)
{
    AppT *app_p = ctx;
    DrawParamView(app_p->parameters_p);
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Continuous Turing Machine");
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    ProgramT* program_p = getProgram();
    app.program_p = program_p;
    InitState(&parameters, program_p);

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

    addPanel(&panels, (PanelT){
        .name = "Simulation",
        .update = updateSimulationPanel,
        .draw = drawSimulationPanel,
        .ctx = &app,
    });
    addPanel(&panels, (PanelT){
        .name = "Edit",
        .update = updateEditPanel,
        .draw = drawEditPanel,
        .ctx = &app,
    });
    addPanel(&panels, (PanelT){
        .name = "Settings",
        .update = NULL,
        .draw = drawParamPanel,
        .ctx = &app,
    });

    while (!WindowShouldClose())
    {
        app.cursor = MOUSE_CURSOR_DEFAULT;

        SetMouseOffset(-mainPane.x, -mainPane.y);
        updateActivePanel(&panels);
        SetMouseCursor(app.cursor);

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

    DestroyState();
    program_p->Destroy();

    CloseWindow();
    return 0;
}
