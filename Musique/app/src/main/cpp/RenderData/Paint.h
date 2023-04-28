#ifndef MUSIQUE_PAINT_H
#define MUSIQUE_PAINT_H

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

        centerVertically = paint.centerVertically;
        centerHorizontally = paint.centerHorizontally;

        hasBackground = paint.hasBackground;
        backgroundColor = paint.backgroundColor;
        backgroundPadding = paint.backgroundPadding;

        verticalEnds = paint.verticalEnds;

        isDottedLine = paint.isDottedLine;
        isDashedLine = paint.isDashedLine;
        dashLength = paint.dashLength;
        dashSpaceLength = paint.dashSpaceLength;
        dotRadius = paint.dotRadius;
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

    bool centerVertically = false;
    bool centerHorizontally = false;

    bool hasBackground = false;
    int backgroundColor = 0xFFFFFFFF;
    float backgroundPadding = 0.0f;

    bool useMusicFont = false;
    bool verticalEnds = false;

    bool isDottedLine = false;
    bool isDashedLine = false;

    // for dashed lines
    float dashLength = 10.0f;

    // for dotted and dashed lines
    float dashSpaceLength = 10.0f;

    // for dotted lines
    float dotRadius = 1.0f;
};

#endif // MUSIQUE_PAINT_H