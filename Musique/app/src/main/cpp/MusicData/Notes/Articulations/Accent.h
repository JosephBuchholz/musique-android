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

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;

    void ModifyVelocity(float& velocity) override;

public:
    enum class Type {
        None = 0, Accent, StrongAccent, SoftAccent
    } type = Type::Accent;

    UpDown facing = UpDown::Up;
};

#endif //MUSIQUE_ACCENT_H