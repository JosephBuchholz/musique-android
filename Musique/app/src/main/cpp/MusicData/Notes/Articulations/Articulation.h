/**
 * This file defines the Articulation base class.
 */

#ifndef MUSIQUE_ARTICULATION_H
#define MUSIQUE_ARTICULATION_H

#include "../../BaseElements/VisibleElement.h"
#include "../NoteStem.h"

/**
 * The base class for musical articulations (such as staccato or tenuto).
 */
class Articulation : public VisibleElement
{

public:

    /**
     * Renders this class.
     *
     * @param[out] renderData The RenderData object to render to.
     * @param[in] notePosition The position of the parent note.
     */
    virtual void Render(RenderData& renderData, Vec2<float> notePosition) const = 0;

    /**
     * Same as the 'Render' function except for debug rendering.
     */
    virtual void RenderDebug(RenderData& renderData, Vec2<float> notePosition) const;

    virtual BoundingBox GetBoundingBox() const = 0;

    virtual Vec2<float> GetDimensions() const = 0;

    /**
     * Calculates positioning variables when in paged mode.
     *
     * @param displayConstants display constants for positioning
     * @param topStaffLineDistNote
     * @param isTab Whether the parent note is a tablature note.
     */
    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) = 0;

    // ----- Sound Related Functions -----

    virtual void ModifySoundDuration(float& soundDuration) {}
    virtual void ModifyVelocity(float& velocity) {}

public:
    // Whether it this articulation is above or below the note.
    AboveBelowType placement = AboveBelowType::Above;

    // x is relative to the left-hand side of the note (the current position in the measure).
    // y is relative to the top line of the staff.

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif //MUSIQUE_ARTICULATION_H