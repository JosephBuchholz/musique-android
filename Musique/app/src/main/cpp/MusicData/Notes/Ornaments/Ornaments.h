/**
 * This file defines the 'Ornament' base class.
 */

#ifndef MUSIQUE_ORNAMENTS_H
#define MUSIQUE_ORNAMENTS_H

#include "../../BaseElements/VisibleElement.h"
#include "../../Types.h"

/**
 * The base class for musical ornaments (such as trills, turns, or tremolos).
 */
class Ornament : public VisibleElement
{
    friend class Note;

public:

    /**
     * Renders this class.
     *
     * @param[out] renderData The RenderData object to render to.
     * @param[in] notePositionX The x position of the parent note.
     * @param[in] topStaffLinePositionY The y position of the top staff line.
     * @param[in] offsetX An offset.
     */
    virtual void Render(RenderData& renderData, float notePositionX, float topStaffLinePositionY, Vec2<float> offset = { 0.0f, 0.0f }) const = 0;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    virtual void RenderDebug(RenderData& renderData) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPositionCenter The position of the parent.
     */
    virtual void UpdateBoundingBox(const Vec2<float>& parentPosition) = 0;

    virtual Vec2<float> GetDimensions() const = 0;

protected:

    /**
     * Calculates positioning variables when in paged mode.
     *
     * @param displayConstants Display constants for positioning.
     * @param defaultPosition The default position.
     */
    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition) = 0;

public:
    // Whether it this ornament is above or below the note.
    AboveBelowType placement = AboveBelowType::Above;

    // x is relative to the left-hand side of the note (the current position in the measure).
    // y is relative to the top line of the staff.
    Vec2<float> position = { 0.0f, 0.0f };

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif //MUSIQUE_ORNAMENTS_H
