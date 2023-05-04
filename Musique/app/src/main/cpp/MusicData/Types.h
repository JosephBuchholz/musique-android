#ifndef MUSIQUE_TYPES_H
#define MUSIQUE_TYPES_H

#include <vector>
#include <string>
#include <limits>
#include "../Debuging/AndroidDebug.h"

#define INVALID_FLOAT_VALUE std::numeric_limits<float>::infinity()
#define INVALID_DOUBLE_VALUE std::numeric_limits<double>::infinity()

typedef float Alter;
typedef unsigned int StringValue;
typedef unsigned int FretValue;

enum class NoteValue {
    None = 0, Maxima, Long, Breve, Whole, Half, Quarter, Eighth, Sixteenth, ThirtySecond, _64th, _128th, _256th, _512th, _1024th
};

struct NoteUnit {
    NoteValue noteValue = NoteValue::None;
    bool isDotted = false;
};

enum class StartStopType {
    None = 0, Start, Stop, Continue
};

enum class AboveBelowType {
    None = 0, Above, Below
};

enum class RightLeftType {
    None = 0, Right, Left
};

enum class Justify {
    None = 0, Left, Right, Center
};

enum class Orientation {
    None = 0, Vertical, Horizontal
};

struct Date {
    int year = 1900;
    int month = 1;
    int day = 1;
};

struct Color {

    Color() {}

    Color(unsigned int color)
        : color(color) {}

    operator int() const { return color; }

    operator unsigned int() const { return color; }

    unsigned int color = 0xff000000;
};

enum class TablatureDisplayType {
    None = 0, NoRhythm, Full
};

struct FontFamily {
    std::vector<std::string> fonts;
};

struct FontSize {

    void SetCSSSize(const std::string& s)
    {
        if (s == "xx-small")
            cssSize = CSSSize::XXSmall;
        else if (s == "x-small")
            cssSize = CSSSize::XXSmall;
        else if (s == "small")
            cssSize = CSSSize::XXSmall;
        else if (s == "medium")
            cssSize = CSSSize::XXSmall;
        else if (s == "large")
            cssSize = CSSSize::XXSmall;
        else if (s == "x-large")
            cssSize = CSSSize::XXSmall;
        else if (s == "xx-large")
            cssSize = CSSSize::XXSmall;
        else
            cssSize = CSSSize::None;

        float medium = 10.0f;
        switch (cssSize)
        {
            case CSSSize::XXSmall: size = medium * 3.0f/5.0f; break;
            case CSSSize::XSmall: size = medium * 3.0f/4.0f; break;
            case CSSSize::Small: size = medium * 8.0f/9.0f; break;
            case CSSSize::Medium: size = medium; break;
            case CSSSize::Large: size = medium * 6.0f/5.0f; break;
            case CSSSize::XLarge: size = medium * 3.0f/2.0f; break;
            case CSSSize::XXLarge: size = medium * 2.0f/1.0f; break;
            default: size = medium; break;
        }

        isCSSSize = true;
    }

    float size = 10.0f;
    enum class CSSSize {
        None = 0, XXSmall, XSmall, Small, Medium, Large, XLarge, XXLarge
    } cssSize = CSSSize::None;
    bool isCSSSize = false;
};

enum class FontStyle {
    None = 0, Normal, Italic
};

enum class FontWeight {
    None = 0, Normal, Bold
};

/**
 * How a line should display.
 */
enum class LineType
{
    None = 0, Dashed, Dotted, Solid, Wavy
};

/**
 * Indicates whether something is up or down
 */
enum class UpDown
{
    None = 0, Up, Down
};

/**
 * Whether a curve is overhand (tips down) or underhand (tips up). (for slurs and ties)
 */
enum class CurveOrientation
{
    None = 0, Over, Under
};

#endif // MUSIQUE_TYPES_H