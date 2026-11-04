#include <raylib.h>

#include "lib/budget.h"

void runWithinBudget(const ParametersT *runtime, void (*step)(void *data), void *data)
{
    double frameStart = GetTime();

    int steps = 0;
    while ((GetTime() - frameStart) < runtime->simBudget*GetFrameTime()
           && steps < runtime->maxSteps)
    {
        step(data);
        steps++;
    }
}
