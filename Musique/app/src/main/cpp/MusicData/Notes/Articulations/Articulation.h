/**
 * This file defines the Articulation base class.
 */

#ifndef MUSIQUE_ARTICULATION_H
#define MUSIQUE_ARTICULATION_H

#include "../../VisibleElement.h"

/**
 * The base class for musical articulations (such as staccato or tenuto).
 */
class Articulation : public VisibleElement
{
    friend class Note;

public:

    /**
     * Renders this class
     *
     * @param[out] renderData The RenderData object to render to
     * @param[in] notePositionX The x position of the parent note
     * @param[in] notePositionY The y position of the parent note
     * @param[in] offsetX offset in x direction
     * @param[in] offsetY offset in y direction
     */
    virtual void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const = 0;

    virtual Vec2<float> GetDimensions() const = 0;

protected:

    /**
     * Calculates positioning variables when in paged mode.
     *
     * @param displayConstants display constants for positioning
     * @param isTab Whether the parent note is a tablature note.
     * @param topStaffLineY The y position for the top staff line (needed for tab only).
     */
    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab) = 0;

public:
    // Whether it this articulation is above or below the note.
    AboveBelowType placement = AboveBelowType::Above;

    // x is relative to the left-hand side of the note (the current position in the measure).
    // y is relative to the top line of the staff.
    Vec2<float> position = { 0.0f, 0.0f };

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif //MUSIQUE_ARTICULATION_H