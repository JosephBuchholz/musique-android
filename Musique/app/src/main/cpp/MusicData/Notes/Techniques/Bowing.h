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

    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

protected:

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab) override;

public:

    enum class Direction {
        None = 0, Up, Down
    } direction = Direction::None;
};

#endif // MUSIQUE_BOWING_H