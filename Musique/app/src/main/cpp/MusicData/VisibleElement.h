#ifndef MUSIQUE_VISIBLEELEMENT_H
#define MUSIQUE_VISIBLEELEMENT_H

#include "PrintableElement.h"
#include "Types.h"

// the base class of all notation elements
class VisibleElement : public PrintableElement {

public:
    VisibleElement() {}

public:
    Color color = Color();
};

#endif // MUSIQUE_VISIBLEELEMENT_H