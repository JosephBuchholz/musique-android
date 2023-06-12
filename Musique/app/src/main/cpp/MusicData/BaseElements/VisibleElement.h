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
    void ModifyPaint(Paint& paint) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    virtual void RenderDebug(RenderData& renderData) const;

    /**
     * The function to call when this object needs to be moved (or resized). This
     * is specifically for bounding boxes/collisions.
     *
     * @param positionOffset The offset for the position.
     * @param sizeOffset The offset for the size.
     * @param rotationOffset The offset for the rotation.
     */
    virtual void Move(Vec2<float> positionOffset, Vec2<float> sizeOffset = { 0.0f, 0.0f }, float rotationOffset = 0.0f);

public:
    Color color = Color();

    // relative to the parent
    Vec2<float> position = { 0.0f, 0.0f };

    float sizeFactor = 1.0f;

    float rotation = 0.0f;

    BoundingBox boundingBox;

#if DEBUG_BOUNDING_BOXES
    BoundingBox debugBoundingBox;
#endif
};

#endif // MUSIQUE_VISIBLEELEMENT_H