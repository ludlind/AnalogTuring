#ifndef __HISTORY_H
#define __HISTORY_H

#include <stdbool.h>

typedef struct HistoryBinsS {
    int steps;
    int capacity;
    int columns;
    int stride;
} HistoryBinsT;

HistoryBinsT historyBins(int steps, int capacity, int columns);
bool  historyColumnSamples(const HistoryBinsT *bins, int column, int *first, int *last);
bool  historyColumnRange(const HistoryBinsT *bins, const float *history, int column, float *low, float *high);
int   historySlot(const HistoryBinsT *bins, int sample);
float historyWindowStart(const HistoryBinsT *bins, float dt);
float historyWindowEnd(const HistoryBinsT *bins, float dt);

#endif
