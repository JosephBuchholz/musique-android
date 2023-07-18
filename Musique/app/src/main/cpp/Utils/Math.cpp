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

bool DoBoundsCollide(int start1, int end1, int start2, int end2)
{
    return (start1 >= start2 && end1 <= end2 ||     // completely inside
               start1 <= start2 && end1 >= end2 ||     // completely outside
               start1 <= start2 && end1 >= start2 ||   // partly left
               start1 <= end2 && end1 >= end2);         // partly right
}