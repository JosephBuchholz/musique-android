#include "Math.h"

#include <cmath>

float DegreesToRadians(float degrees)
{
    return degrees * ((2 * M_PI) / 360.0f);
}

float RadiansToDegrees(float radians)
{
    return radians * (360.0f / (2 * M_PI));
}