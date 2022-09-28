#pragma once

#include <vector>
#include <string>
#include "../AndroidDebug.h"

enum class StartStopType {
    None = 0, Start, Stop, Continue
};

enum class AboveBelowType {
    None = 0, Above, Below
};

enum class Justify {
    None = 0, Left, Right, Center
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
        size = 1.0f;
        LOGW("Haven't implemented css font sizes");
        isCSSSize = true;
    }

    float size = 0.0f;
    bool isCSSSize = false;
};

enum class FontStyle {
    None = 0, Normal, Italic
};

enum class FontWeight {
    None = 0, Normal, Bold
};