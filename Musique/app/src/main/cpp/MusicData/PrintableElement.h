#pragma once
#include "BaseElement.h"

// the base class of all notation elements
class PrintableElement : public BaseElement {

public:
    PrintableElement() {}

public:
    bool printObject = true;
};
