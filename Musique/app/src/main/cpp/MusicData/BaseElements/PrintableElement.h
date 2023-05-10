#ifndef MUSIQUE_PRINTABLEELEMENT_H
#define MUSIQUE_PRINTABLEELEMENT_H

#include "BaseElement.h"

/**
 * This class represents a element that is able to be rendered.
 */
class PrintableElement : public virtual BaseElement {

public:
    PrintableElement() {}

public:
    bool printObject = true;
};

#endif // MUSIQUE_PRINTABLEELEMENT_H