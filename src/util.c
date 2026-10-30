#include <math.h>

#include "util.h"

float gaussian(float dx, float sigma)
{
    float N = 1.0f/sqrt(2.0f*PI)/sigma;
    float a = dx/sigma;
    return N * expf(-a*a/2.0f);
}
