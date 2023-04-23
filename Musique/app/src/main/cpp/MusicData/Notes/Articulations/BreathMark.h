/**
 * This files defines the BreathMark class.
 */

#ifndef MUSIQUE_BREATHMARK_H
#define MUSIQUE_BREATHMARK_H

#include "Articulation.h"

/**
 * This class represents a breath mark.
 */
class BreathMark : public Articulation
{
public:

    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

protected:

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab) override;
};

#endif //MUSIQUE_BREATHMARK_H
