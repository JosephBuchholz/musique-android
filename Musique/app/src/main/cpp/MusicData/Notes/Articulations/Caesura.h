/**
 * This file defines the Caesura class.
 */

#ifndef MUSIQUE_CAESURA_H
#define MUSIQUE_CAESURA_H

#include "Articulation.h"

/**
 * This class represents a musical caesura (the "train-tracks").
 */
class Caesura : public Articulation
{
public:

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;

public:

    enum class Type {
        None = 0, Normal, Thick, Short, Curved, Single
    } type = Type::Normal;
};

#endif //MUSIQUE_CAESURA_H
