#ifndef MUSIQUE_MATH_H
#define MUSIQUE_MATH_H

#include "../Collisions/Vec2.h"

float DegreesToRadians(float degrees);
float RadiansToDegrees(float radians);

float FindDistance(Vec2<float> v1, Vec2<float> v2);

bool DoBoundsCollide(int start1, int end1, int start2, int end2);

int GetSign(int x);

#endif //MUSIQUE_MATH_H
