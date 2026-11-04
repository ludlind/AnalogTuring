#ifndef __BUDGET_H
#define __BUDGET_H

#include "common.h"

void runWithinBudget(const ParametersT *runtime, void (*step)(void *data), void *data);

#endif
