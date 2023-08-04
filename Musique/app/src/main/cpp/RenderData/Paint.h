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
        glyphSizeFactor = paint.glyphSizeFactor;
        align = paint.align;
        isItalic = paint.isItalic;
        isBold = paint.isBold;
        isTablature = paint.isTablature;
        isAntiAlias = paint.isAntiAlias;
        strikeThruText = paint.strikeThruText;

        rotateDegrees = paint.rotateDegrees;

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

        varyThickness = paint.varyThickness;
    }

    enum class Cap {
        Butt = 0, Round, Square
    };

    enum class Align {
        Left = 0, Center, Right
    };

    long color = 0xff000000;

    float strokeWidth = 1.0f;
    Cap strokeCap = Cap::Butt;
    float textSize = 10.0f;
    float glyphSizeFactor = 1.0f; // this scales the size of the glyph
    Align align = Align::Center;
    bool isItalic = false;
    bool isBold = false;
    bool isTablature = false;
    bool isAntiAlias = false;
    bool strikeThruText = false;

    float rotateDegrees = 0.0f; // how much to rotate by in degrees

    bool centerVertically = false;
    bool centerHorizontally = false;

    bool hasBackground = false;
    long backgroundColor = 0xFFFFFFFF;
    float backgroundPadding = 0.0f;

    bool useMusicFont = false;
    bool verticalEnds = false;

    bool isDottedLine = false;
    bool isDashedLine = false;

    bool varyThickness = false;

    // for dashed lines
    float dashLength = 10.0f;

    // for dotted and dashed lines
    float dashSpaceLength = 10.0f;

    // for dotted lines
    float dotRadius = 1.0f;
};

#endif // MUSIQUE_PAINT_H