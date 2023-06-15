/**
 * This file defines the Accent class.
 */

#ifndef MUSIQUE_ACCENT_H
#define MUSIQUE_ACCENT_H

#include "../../Types.h"
#include "../../../Collisions/Vec2.h"
#include "Articulation.h"

/**
 * This class represents an accent marking.
 */
class Accent : public Articulation
{
public:

    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

protected:

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab) override;

public:
    enum class Type {
        None = 0, Accent, StrongAccent, SoftAccent
    } type = Type::Accent;

    UpDown facing = UpDown::Up;
};

#endif //MUSIQUE_ACCENT_H