#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "state.h"
#include "models/model0.h"
#include "models/common.h"
#include "models/model1.h"
#include "models/model2.h"
#include "program.h"

#define USE_MODEL 2

static ProgramT programs[];

float GetStartS(void)
{
    return programs[USE_MODEL].startS;
}

void SetStartS(float S)
{
    programs[USE_MODEL].startS = S;
}

static ProgramT programs[] = {
    {initModel0, destroyProgram, initState,
     F_S, F_X, F_M, dMdt, 0.0f},
    {initModel1, destroyProgram, initState,
     gaussian_states_F_S, gaussian_states_F_X,
     gaussian_states_F_M, gaussian_states_dMdt, 0.0f},
    {initModel2, destroyProgram, initState,
     pixel_F_S, pixel_F_X, pixel_F_M, pixel_dMdt, 0.0f},
};

ProgramT *getProgram(void)
{
    static bool initiated = false;
    if (!initiated)
    {
        programs[USE_MODEL].Init();
        initiated = true;
    }

    return &programs[USE_MODEL];
}
