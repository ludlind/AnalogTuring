#include "lib/history.h"

static int newestBlock(const HistoryBinsT *bins)
{
    return (bins->steps > 0) ? (bins->steps - 1)/bins->stride : 0;
}

HistoryBinsT historyBins(int steps, int capacity, int columns)
{
    return (HistoryBinsT){
        .steps = steps,
        .capacity = capacity,
        .columns = columns,
        .stride = capacity/columns,
    };
}

bool historyColumnSamples(const HistoryBinsT *bins, int column, int *first, int *last)
{
    int block = newestBlock(bins) - (bins->columns - 1) + column;
    if (bins->steps == 0 || block < 0)
    {
        return false;
    }

    *first = block*bins->stride;
    *last = *first + bins->stride - 1;
    if (*last >= bins->steps)
    {
        *last = bins->steps - 1;
    }
    return *first <= *last;
}

bool historyColumnRange(const HistoryBinsT *bins, const float *history, int column, float *low, float *high)
{
    int first, last;
    if (!historyColumnSamples(bins, column, &first, &last))
    {
        return false;
    }

    *low = *high = history[historySlot(bins, first)];
    for (int sample = first + 1; sample <= last; sample++)
    {
        float value = history[historySlot(bins, sample)];
        if (value < *low)
        {
            *low = value;
        }
        if (value > *high)
        {
            *high = value;
        }
    }
    return true;
}

int historySlot(const HistoryBinsT *bins, int sample)
{
    return sample % bins->capacity;
}

float historyWindowEnd(const HistoryBinsT *bins, float dt)
{
    return (newestBlock(bins) + 1)*bins->stride*dt;
}

float historyWindowStart(const HistoryBinsT *bins, float dt)
{
    return (newestBlock(bins) + 1 - bins->columns)*bins->stride*dt;
}
