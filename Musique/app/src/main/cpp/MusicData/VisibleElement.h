#pragma once
#include "PrintableElement.h"
#include "Types.h"

// the base class of all notation elements
class VisibleElement : public PrintableElement {

public:
    VisibleElement() {}

public:
    Color color = Color();
};
