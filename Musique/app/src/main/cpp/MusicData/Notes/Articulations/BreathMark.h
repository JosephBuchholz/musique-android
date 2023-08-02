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

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;
};

#endif //MUSIQUE_BREATHMARK_H
