#ifndef __MODEL_1D_MODEL_H
#define __MODEL_1D_MODEL_H

#include <stdbool.h>
#include <stdlib.h>
#include <raylib.h>

#include "common.h"

#define MEMORY_GRID_POINTS 200001
#define MEMORY_GRID_SIZE 0.00001f
#define MEMORY_GRID_OFFSET -1.0f

#define MEMORY_START_POSITION MEMORY_GRID_OFFSET
#define MEMORY_STOP_POSITION  (MEMORY_GRID_OFFSET + (MEMORY_GRID_POINTS-1) * MEMORY_GRID_SIZE)

#define MEMORY_MIN_VALUE -1.0f
#define MEMORY_MAX_VALUE 1.0f

#define STATE_MIN_VALUE -1.0f
#define STATE_MAX_VALUE 1.0f

#define TIME_MEMORY_GRID_POINTS 10001

#define NUMBER_OF_COLUMNS 20
#define NUMBER_OF_ROWS    20
#define NUMBER_OF_STATES  10

typedef struct StateS {
    float S;
    float X;
    float *M;
    float *Mnext;
    float m;
    float sigma_M;
    float dt;
    float t;
} StateT;

typedef struct MemoryS {
    int   currentMemIndex;
    float currentMemTime;
    bool  wrapMem;
    float SMem[TIME_MEMORY_GRID_POINTS];
    float XMem[TIME_MEMORY_GRID_POINTS];
    float mMem[TIME_MEMORY_GRID_POINTS];
} MemoryT;

typedef struct Model1DDataS Model1DDataT;

typedef struct VelocityFieldS {
    void  (*init)(Model1DDataT *data);
    float (*F_S)(const Model1DDataT *data, float S, float m);
    float (*F_X)(const Model1DDataT *data, float S, float m);
    float (*F_M)(const Model1DDataT *data, float S, float m);
} VelocityFieldT;

typedef struct Model1DParametersS {
    float dt;
    float sigma_M;
} Model1DParametersT;

struct Model1DDataS {
    const VelocityFieldT *velocity;
    const ParametersT *runtime;
    Model1DParametersT parameters;

    StateT  state;
    MemoryT memory;

    int    stateMap[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];
    float  FS[NUMBER_OF_STATES];
    float  FX[NUMBER_OF_STATES];
    float  FM[NUMBER_OF_STATES];
    float *initM;
    float  startS;

    int   selectedState;
    float brushSize;
};

static inline Model1DDataT *model1d_of(void *p) { return p; }

#endif
