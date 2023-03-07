#ifndef MUSIQUE_PRINTABLEELEMENT_H
#define MUSIQUE_PRINTABLEELEMENT_H

#include "BaseElement.h"

// the base class of all notation elements
class PrintableElement : public BaseElement {

public:
    PrintableElement() {}

public:
    bool printObject = true;
};

#endif // MUSIQUE_PRINTABLEELEMENT_H