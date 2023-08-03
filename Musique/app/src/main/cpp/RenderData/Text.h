#ifndef MUSIQUE_RENDERTEXT_H
#define MUSIQUE_RENDERTEXT_H

#include <string>
#include "Paint.h"
#include "../Collisions/Vec2.h"

/**
 * An object that holds data needed to render text.
 */
struct Text
{

    Text() {}

    Text(const std::string& text, Vec2<float> position, const Paint& paint = Paint())
            : text(text), position(position), paint(paint) {
    }

    Text(const std::string& text, float positionX, float positionY, const Paint& paint = Paint())
            : text(text), position({ positionX, positionY }), paint(paint) {
    }

    std::string text = "";
    Vec2<float> position = { 0.0f, 0.0f };
    Paint paint = Paint();
};

#endif // MUSIQUE_RENDERTEXT_H