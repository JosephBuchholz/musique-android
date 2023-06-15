/**
 * This file contains the definition for the `GuitarTechnique` class.
 */

#ifndef MUSIQUE_GUITARTECHNIQUE_H
#define MUSIQUE_GUITARTECHNIQUE_H

#include "Technique.h"

/**
 * This class represents some techniques for guitar (such as tapping).
 */
class GuitarTechnique : public Technique
{
public:

    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

protected:

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab) override;

public:

    enum class Type {
        None = 0, Tap, Golpe
    } type = Type::None;

    // for tapping only
    enum class Hand {
        None = 0, Left, Right
    } hand = Hand::Right;
};

#endif //MUSIQUE_GUITARTECHNIQUE_H
