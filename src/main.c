#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "common.h"
#include "raygui.h"
#include "state.h"
#include "program.h"
#include "simview.h"
#include "editview.h"
#include "paramview.h"

typedef enum {
    MODE_SIMULATION = 0,
    MODE_EDIT,
    MODE_PARAM,
    NUMBER_OF_MODES
} ApplicationModeE;

char modeNames[NUMBER_OF_MODES][32] = {
    "Simulation View",
    "Edit View",
    "Parameter View"
};

ParametersT parameters = {
    0.3,
    100,
    0.001f,
    0.01f,
};

AppT app = {
    .cursor = MOUSE_CURSOR_DEFAULT,
};

ApplicationModeE mode = MODE_SIMULATION;

static Rectangle menuPane = {
    0, 0, WIDTH, 50
};
static Rectangle mainPane = {
    0, 50, WIDTH, HEIGHT - 50
};

void DrawMenu(void);

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Continuous Turing Machine");
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    ProgramT* program_p = getProgram();
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

    while (!WindowShouldClose())
    {
        double frameStart = GetTime();
        app.cursor = MOUSE_CURSOR_DEFAULT;

        SetMouseOffset(-mainPane.x, -mainPane.y);
        switch (mode)
        {
        case MODE_SIMULATION:
            if (IsKeyPressed(KEY_R))
            {
                InitState(&parameters, program_p);
            }

            int steps = 0;
            while ((GetTime() - frameStart) < parameters.simBudget*GetFrameTime() && steps < parameters.maxSteps) {
                UpdateState(program_p);
                steps++;
            }
            break;
        case MODE_EDIT:
            HandleInputEditView(&app, program_p);
            break;
        default:
            // do nothing
        }
        SetMouseCursor(app.cursor);

        BeginDrawing();
        ClearBackground((Color){ 8, 10, 14, 255 }); 
        BeginScissorMode(mainPane.x, mainPane.y, mainPane.width, mainPane.height);
        BeginMode2D(mainPaneCamera);
        SetMouseOffset(-mainPane.x, -mainPane.y);
        switch (mode)
        {
        case MODE_SIMULATION:
            DrawSimulationView(program_p);
            break;
        case MODE_EDIT:
            DrawEditView(program_p);
            break;
        case MODE_PARAM:
            DrawParamView(&parameters);
        default:
            // do nothing
        }
        EndMode2D();
        EndScissorMode();
        BeginScissorMode(menuPane.x, menuPane.y, menuPane.width, menuPane.height);
        BeginMode2D(menuPaneCamera);
        SetMouseOffset(-menuPane.x, -menuPane.y);
        DrawMenu();
        EndMode2D();
        EndScissorMode();
        EndDrawing();
    }

    DestroyState();
    program_p->Destroy();

    CloseWindow();
    return 0;
}

void DrawMenu(void)
{
    /*for (int i = 0; i < NUMBER_OF_MODES; i++)
    {
        if (GuiToggle((Rectangle){120*i + 20, 10, 110, 20}, modeNames[i], mode == i))
        {
            mode = i; 
        }
    }*/
    GuiToggleGroup(
        (Rectangle){20, 20, 160, 20},
        "Simulation;Edit;Settings",
        &mode);
}
