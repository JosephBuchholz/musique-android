#pragma once
#include "Paint.h"
#include "SMuFLID.h"

struct SMuFLGlyph {

    SMuFLGlyph() {}

    SMuFLGlyph(SMuFLID codePoint, float x, float y, Paint paint = Paint())
            : codePoint(codePoint), x(x), y(y), paint(paint) {
    }

    SMuFLID codePoint = SMuFLID::None;
    float x = 0.0f;
    float y = 0.0f;
    Paint paint = Paint();
};