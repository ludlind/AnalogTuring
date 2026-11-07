#ifndef __MODEL_2D_MODEL_H
#define __MODEL_2D_MODEL_H

#include <stdbool.h>
#include <stdlib.h>
#include <complex.h>
#include <raylib.h>

#include "common.h"

#define MEMORY_GRID_POINTS 200001
#define MEMORY_GRID_SIZE 0.00001f
#define MEMORY_GRID_OFFSET -1.0f

#define MEMORY_START_POSITION MEMORY_GRID_OFFSET
#define MEMORY_STOP_POSITION  (MEMORY_GRID_OFFSET + (MEMORY_GRID_POINTS-1) * MEMORY_GRID_SIZE)

#define MEMORY_MIN_VALUE -1.0f
#define MEMORY_MAX_VALUE 1.0f

#define STATE_MAX_RADIUS 1.0f

#define FIELD_MIN -1.0f
#define FIELD_MAX  1.0f

#define TIME_MEMORY_GRID_POINTS 10001

typedef struct State2DS {
    float complex S;
    float theta;
    float X;
    float *M;
    float *Mnext;
    float m;
    float sigma_M;
    float omega;
    float dt;
    float t;
} State2DT;

typedef struct Memory2DS {
    int   currentMemIndex;
    float currentMemTime;
    bool  wrapMem;
    float complex SMem[TIME_MEMORY_GRID_POINTS];
    float thetaMem[TIME_MEMORY_GRID_POINTS];
    float XMem[TIME_MEMORY_GRID_POINTS];
    float mMem[TIME_MEMORY_GRID_POINTS];
} Memory2DT;

typedef enum SliceVarE {
    SLICE_RE_S,
    SLICE_IM_S,
    SLICE_ABS_S,
    SLICE_ARG_S,
    SLICE_M,
    SLICE_THETA,
    SLICE_VAR_COUNT,
} SliceVarT;

typedef enum SliceFieldE {
    SLICE_F_S,
    SLICE_F_M,
    SLICE_F_X,
} SliceFieldT;

typedef struct SliceViewS {
    int   xAxis;
    int   yAxis;
    int   field;
    int   polarSliders;
    bool  follow;
    float values[SLICE_VAR_COUNT];
} SliceViewT;

typedef struct Model2DDataS Model2DDataT;

typedef struct VelocityField2DS {
    void          (*init)(Model2DDataT *data);
    float complex (*F_S)(const Model2DDataT *data, float complex S, float m, float theta);
    float         (*F_X)(const Model2DDataT *data, float complex S, float m, float theta);
    float         (*F_M)(const Model2DDataT *data, float complex S, float m, float theta);
} VelocityField2DT;

typedef struct Model2DParametersS {
    float dt;
    float sigma_M;
    float omega;
} Model2DParametersT;

struct Model2DDataS {
    const VelocityField2DT *velocity;
    const ParametersT *runtime;
    Model2DParametersT parameters;

    State2DT  state;
    Memory2DT memory;

    float *initM;
    float complex startS;

    SliceViewT slice;
};

static inline Model2DDataT *model2d_of(void *p) { return p; }

#endif
