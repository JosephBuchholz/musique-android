#pragma once
#include "BaseElement.h"
#include "Types.h"

// the base class of all notation elements
class VisibleElement : public BaseElement {

public:
    VisibleElement() {}

public:
    Color color = Color();
};
