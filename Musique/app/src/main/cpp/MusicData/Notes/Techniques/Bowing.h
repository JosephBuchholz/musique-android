/**
 * This file contains the definition for the `Bowing` class.
 */

#ifndef MUSIQUE_BOWING_H
#define MUSIQUE_BOWING_H

#include "Technique.h"

/**
 * This class represents up and down bowing notation (also up and down picking).
 */
class Bowing : public Technique
{
public:

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override { return BoundingBox(); }

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;

public:

    enum class Direction {
        None = 0, Up, Down
    } direction = Direction::None;
};

#endif // MUSIQUE_BOWING_H