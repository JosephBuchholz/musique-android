#ifndef MUSIQUE_VISIBLEELEMENT_H
#define MUSIQUE_VISIBLEELEMENT_H

#include "PrintableElement.h"
#include "../Types.h"
#include "../../Collisions/BoundingBox.h"
#include "../../Collisions/Vec2.h"
#include "../../Debuging/Debug.h"

/**
 * This class represents a element that is visible.
 */
class VisibleElement : public PrintableElement {

public:
    VisibleElement() {}

    /**
     * Updates the given paint to match the attributes of this class.
     *
     * @param[out] paint The paint object to be modified.
     */
    void ModifyPaint(Paint& paint) const { paint.color = color.color; }

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    virtual void RenderDebug(RenderData& renderData) const;

public:
    Color color = Color();

    BoundingBox boundingBox;

#if DEBUG_BOUNDING_BOXES
    BoundingBox debugBoundingBox;
#endif
};

#endif // MUSIQUE_VISIBLEELEMENT_H