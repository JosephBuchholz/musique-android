#pragma once
#include "VisibleElement.h"
#include "Types.h"

// the base class of all notation elements
class TextualElement : public VisibleElement {

public:
    TextualElement() {}

public:
    FontFamily fontFamily = FontFamily();
    FontSize fontSize = FontSize();
    FontStyle fontStyle = FontStyle::Normal;
    FontWeight fontWeight = FontWeight::Normal;

    Justify justify = Justify::Left;

    unsigned int linesThrough = 0;
    unsigned int overline = 0;
    unsigned int underline = 0;
};
