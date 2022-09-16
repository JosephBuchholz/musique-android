#pragma once

#include "Paint.h"

// a cubic bezier curve
struct CubicCurve {

    CubicCurve() {}

    CubicCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, Paint paint = Paint())
            : x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3), x4(x4), y4(y4), paint(paint) {
    }

    float x1 = 0.0f;
    float y1 = 0.0f;

    float x2 = 0.0f;
    float y2 = 0.0f;

    float x3 = 0.0f;
    float y3 = 0.0f;

    float x4 = 0.0f;
    float y4 = 0.0f;

    Paint paint = Paint();
};