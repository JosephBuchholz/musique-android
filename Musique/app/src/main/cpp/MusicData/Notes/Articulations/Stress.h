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

    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

protected:

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab) override;

public:

    enum class Type {
        None = 0, Stress, Unstress
    } type = Type::Stress;
};

#endif //MUSIQUE_STRESS_H
