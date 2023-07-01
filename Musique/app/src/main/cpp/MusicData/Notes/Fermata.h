/**
 * This file contains the definition for the 'Fermata' class.
 */

#ifndef MUSIQUE_FERMATA_H
#define MUSIQUE_FERMATA_H

#include "../BaseElements/VisibleElement.h"

/**
* This class represents a fermata.
*/
class Fermata : public VisibleElement
{

public:

    void Render(RenderData& renderData, Vec2<float> parentPositionCenter, Vec2<float> offset = { 0.0f, 0.0f }) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

    SMuFLID GetSMuFLID() const;

    BoundingBox GetBoundingBoxRelativeToParent() const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPositionCenter The center position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float>& parentPositionCenter);

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPosition);

public:

    enum class Shape {
        None = 0, Normal, Angled, Square, DoubleAngled, DoubleSquare, DoubleDot, HalfCurve, Curlew
    } shape = Shape::Normal;

    enum class Direction {
        None = 0, Upright, Inverted
    } direction = Direction::Upright;

    // x pos is relative to the center of note or barline
    // y pos is relative to the top staff line

protected:

    // -- Positioning Attributes From MusicXML --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif //MUSIQUE_FERMATA_H
