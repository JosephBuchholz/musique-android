#ifndef MUSIQUE_SMUFLGLYPH_H
#define MUSIQUE_SMUFLGLYPH_H

#include "Paint.h"
#include "SMuFLID.h"
#include "../Collisions/Vec2.h"

/**
 * An object that holds data needed to render a SMuFL (Standard Music Font Layout) glyph.
 */
struct SMuFLGlyph
{

    SMuFLGlyph() {}

    SMuFLGlyph(SMuFLID codePoint, Vec2<float> position, Paint paint = Paint())
            : codePoint(codePoint), position(position), paint(paint) {
    }

    SMuFLGlyph(SMuFLID codePoint, float positionX, float positionY, Paint paint = Paint())
            : codePoint(codePoint), position({ positionX, positionY }), paint(paint) {
    }

    SMuFLID codePoint = SMuFLID::None;
    Vec2<float> position = { 0.0f, 0.0f };
    Paint paint = Paint();
};

#endif // MUSIQUE_SMUFLGLYPH_H