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

    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const override;
    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

protected:

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab) override;

public:

    enum class Type {
        None = 0, Staccato, Tenuto, DetachedLegato, Staccatissimo, Spiccato
    } type = Type::Staccato;
};

#endif //MUSIQUE_STACCATOARTICULATION_H
