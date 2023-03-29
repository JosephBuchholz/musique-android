#ifndef MUSIQUE_VISIBLEELEMENT_H
#define MUSIQUE_VISIBLEELEMENT_H

#include "PrintableElement.h"
#include "Types.h"
#include "../Collisions/BoundingBox.h"
#include "../Collisions/Vec2.h"

/**
 * This class represents a element that is visible.
 */
class VisibleElement : public PrintableElement {

public:
    VisibleElement() {}

public:
    Color color = Color();

    BoundingBox boundingBox;
};

#endif // MUSIQUE_VISIBLEELEMENT_H