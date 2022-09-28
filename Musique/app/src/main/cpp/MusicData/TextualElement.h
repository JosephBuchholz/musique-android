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
};
