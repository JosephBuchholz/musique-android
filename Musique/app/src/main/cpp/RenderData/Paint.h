#pragma once

/**
 * An object that holds data needed to describe how a specific object
 * should be rendered (for example: color, font, or stroke width).
 */
struct Paint {

    Paint() {}

    Paint(int color)
            : color(color) {
    }

    Paint(int color, const Paint& paint)
            : color(color) {
        strokeWidth = paint.strokeWidth;
        strokeCap = paint.strokeCap;
        textSize = paint.textSize;
        align = paint.align;
        isItalic = paint.isItalic;
        isBold = paint.isBold;
        isTablature = paint.isTablature;
        isAntiAlias = paint.isAntiAlias;
        strikeThruText = paint.strikeThruText;
        centerTextVertically = paint.centerTextVertically;
    }

    enum class Cap {
        Butt = 0, Round, Square
    };

    enum class Align {
        Left = 0, Center, Right
    };

    int color = 0xff000000;

    float strokeWidth = 1.0f;
    Cap strokeCap = Cap::Butt;
    float textSize = 10.0f;
    Align align = Align::Center;
    bool isItalic = false;
    bool isBold = false;
    bool isTablature = false;
    bool isAntiAlias = false;
    bool strikeThruText = false;
    bool centerTextVertically = false;
    bool useMusicFont = false;
};