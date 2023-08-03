#ifndef MUSIQUE_CUBICCURVE_H
#define MUSIQUE_CUBICCURVE_H

#include "Paint.h"
#include "../Collisions/Vec2.h"

/**
 * An object that holds data needed to render cubic bezier curve.
 */
struct CubicCurve {

    CubicCurve() {}

    CubicCurve(Vec2<float> point1, Vec2<float> point2, Vec2<float> point3, Vec2<float> point4, Paint paint = Paint())
            : point1(point1), point2(point2), point3(point3), point4(point4), paint(paint) {
    }

    Vec2<float> point1 = { 0.0f, 0.0f };
    Vec2<float> point2 = { 0.0f, 0.0f };
    Vec2<float> point3 = { 0.0f, 0.0f };
    Vec2<float> point4 = { 0.0f, 0.0f };

    Paint paint = Paint();
};

#endif // MUSIQUE_CUBICCURVE_H