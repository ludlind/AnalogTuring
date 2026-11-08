#ifndef __MODEL_2D_MODEL_H
#define __MODEL_2D_MODEL_H

#include <stdbool.h>
#include <stdlib.h>
#include <complex.h>
#include <raylib.h>

#include "common.h"
#include "gui/valuerow.h"

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

#define MAX_KEYFRAMES             16
#define MAX_SYMBOLS               8
#define NUMBER_OF_SPLAT_FUNCTIONS 8
#define MAX_SPLATS                32

#define PARAMETER_ROWS 8

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
    int   steps;
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

typedef enum FieldComponentE {
    FIELD_F_S,
    FIELD_F_M,
    FIELD_F_X,
    FIELD_COMPONENT_COUNT,
} FieldComponentT;

typedef struct SplatS {
    float complex center;
    float sigma;
    float complex value;
    float complex alpha;
    float complex beta;
} SplatT;

typedef struct SplatListS {
    int count;
    SplatT splats[MAX_SPLATS];
} SplatListT;

typedef struct SplatFunctionS {
    SplatListT components[FIELD_COMPONENT_COUNT];
} SplatFunctionT;

typedef struct KeyframeProgramS {
    int keyframes;
    int symbols;
    int index[MAX_KEYFRAMES][MAX_SYMBOLS];
    SplatFunctionT functions[NUMBER_OF_SPLAT_FUNCTIONS];
} KeyframeProgramT;

typedef struct KeyframeEditS {
    int    selectedFunction;
    int    component;
    int    selectedSplat;
    bool   dragging;
    SplatT brush[FIELD_COMPONENT_COUNT];
} KeyframeEditT;

typedef struct InputViewS {
    float brushSize;
} InputViewT;

typedef struct ParameterViewS {
    ValueBoxT boxes[PARAMETER_ROWS];
} ParameterViewT;

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
    float complex startS;
    float startTheta;
    float startX;
    float startm;
} Model2DParametersT;

struct Model2DDataS {
    const VelocityField2DT *velocity;
    const ParametersT *runtime;
    Model2DParametersT parameters;

    State2DT  state;
    Memory2DT memory;

    float *initM;

    KeyframeProgramT program;
    KeyframeEditT    edit;
    InputViewT       input;
    ParameterViewT   parameterView;

    SliceViewT slice;
};

static inline Model2DDataT *model2d_of(void *p) { return p; }

#endif
