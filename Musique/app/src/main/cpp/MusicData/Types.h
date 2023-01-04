#pragma once

#include <vector>
#include <string>
#include "../AndroidDebug.h"

typedef float Alter;
typedef unsigned int StringValue;
typedef unsigned int FretValue;

enum class NoteValue {
    None = 0, Whole, Half, Quarter, Eighth, Sixteenth, ThirtySecond
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
    int color = 0xff000000;
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