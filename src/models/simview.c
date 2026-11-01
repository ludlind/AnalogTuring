#include <raylib.h>

#include "gui/visual.h"
#include "state.h"
#include "models/simview.h"

static Rectangle FSMapGr = { 20, 20, 300, 300 };
static Rectangle FMMapGr = { 340, 20, 300, 300 };
static Rectangle FXMapGr = { 660, 20, 300, 300 };
static Rectangle SMemGr  = { 20, 340, 300, 200 };
static Rectangle XMemGr  = { 340, 340, 300, 200 };
static Rectangle mMemGr  = { 660, 340, 300, 200 };

void DrawSimulationView(ProgramT* program_p)
{
    float S = getS(); 
    float m = getm(); 
   
    float t1 = getMemoryStartTime();
    float t2 = getTime();

    float Smin = getStateMinValue();
    float Smax = getStateMaxValue();
    
    float Xmin = getMemoryStartPosition();
    float Xmax = getMemoryStopPosition();

    float Mmin = getMemoryMinValue();
    float Mmax = getMemoryMaxValue();
   
    DrawMap(FSMapGr, 5, 2, 0, program_p->F_S, Smin, Smax, Mmin, Mmax, -1.0f, 1.0f);
    DrawMap(FMMapGr, 5, 2, 0, program_p->F_M, Smin, Smax, Mmin, Mmax, -1.0f, 1.0f);
    DrawMap(FXMapGr, 5, 2, 0, program_p->F_X, Smin, Smax, Mmin, Mmax, -1.0f, 1.0f);

    DrawDot(FSMapGr, 2, S, m, Smin, Smax, Mmin, Mmax, RAYWHITE);
    DrawDot(FMMapGr, 2, S, m, Smin, Smax, Mmin, Mmax, RAYWHITE);
    DrawDot(FXMapGr, 2, S, m, Smin, Smax, Mmin, Mmax, RAYWHITE);

    DrawScope(SMemGr, 2, interpS, t1, t2, Smin, Smax);
    DrawScope(XMemGr, 2, interpX, t1, t2, Xmin, Xmax);
    DrawScope(mMemGr, 2, interpM, Xmin, Xmax, Mmin, Mmax);
}
