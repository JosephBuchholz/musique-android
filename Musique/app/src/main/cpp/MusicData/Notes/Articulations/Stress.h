/**
 * This file defines the Stress class.
 */

#ifndef MUSIQUE_STRESS_H
#define MUSIQUE_STRESS_H

#include "Articulation.h"

/**
 * This class represents a stress or unstress mark for a note.
 */
class Stress : public Articulation
{
public:

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;

public:

    enum class Type {
        None = 0, Stress, Unstress
    } type = Type::Stress;
};

#endif //MUSIQUE_STRESS_H
