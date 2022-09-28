#pragma once

#include "TextualElement.h"
#include "Types.h"
#include "String.h"

class Words : public TextualElement {

public:
    Words() {  }

public:
    String text;
    Justify justify = Justify::Left;
};