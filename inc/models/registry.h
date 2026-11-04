#ifndef __MODELS_REGISTRY_H
#define __MODELS_REGISTRY_H

#include "common.h"
#include "core/panel.h"

typedef struct ModelS {
    const char *name;
    void *(*create)(const ParametersT *runtime);
    void (*destroy)(void *data);
    void (*addPanels)(void *data, PanelListT *panels);
} ModelT;

int getModelCount(void);
const ModelT *getModel(int modelIndex);

#endif
