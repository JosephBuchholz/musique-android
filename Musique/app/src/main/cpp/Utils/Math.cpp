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

float FindDistance(Vec2<float> v1, Vec2<float> v2)
{
    return std::sqrt(((v1.x - v2.x) * (v1.x - v2.x)) + ((v1.y - v2.y) * (v1.y - v2.y)));
}