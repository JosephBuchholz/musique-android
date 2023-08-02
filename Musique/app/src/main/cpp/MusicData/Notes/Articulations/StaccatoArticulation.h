/**
 * This file defines the StaccatoArticulation class.
 */

#ifndef MUSIQUE_STACCATOARTICULATION_H
#define MUSIQUE_STACCATOARTICULATION_H

#include "Articulation.h"

/**
 * This class represents a staccato type (this includes tenuto) of articulation (like staccato or tenuto or spiccato).
 */
class StaccatoArticulation : public Articulation
{
public:

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;

    void ModifySoundDuration(float& soundDuration) override;
    void ModifyVelocity(float& velocity) override;

public:

    enum class Type {
        None = 0, Staccato, Tenuto, DetachedLegato, Staccatissimo, Spiccato
    } type = Type::Staccato;
};

#endif //MUSIQUE_STACCATOARTICULATION_H
