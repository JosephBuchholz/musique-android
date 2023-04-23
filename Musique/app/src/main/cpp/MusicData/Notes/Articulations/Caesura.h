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

    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

protected:

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab) override;

public:

    enum class Type {
        None = 0, Normal, Thick, Short, Curved, Single
    } type = Type::Normal;
};

#endif //MUSIQUE_CAESURA_H
