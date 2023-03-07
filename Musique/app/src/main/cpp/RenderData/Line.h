#ifndef MUSIQUE_LINE_H
#define MUSIQUE_LINE_H

#include "Paint.h"

/**
 * An object that holds data needed to render a line.
 */
struct Line {

    Line() {}

    Line(float startX, float startY, float endX, float endY, Paint paint = Paint())
        : startX(startX), startY(startY), endX(endX), endY(endY), paint(paint) {
    }

    float startX = 0.0f;
    float startY = 0.0f;

    float endX = 0.0f;
    float endY = 0.0f;

    Paint paint = Paint();
};

#endif // MUSIQUE_LINE_H