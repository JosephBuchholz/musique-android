#pragma once

#include <string>
#include "Paint.h"

/**
 * An object that holds data needed to render text.
 */
struct Text {

    Text() {}

    Text(const std::string& text, float x, float y, Paint paint = Paint())
            : text(text), x(x), y(y), paint(paint) {
    }

    std::string text = "";
    float x = 0.0f;
    float y = 0.0f;

    Paint paint = Paint();
};