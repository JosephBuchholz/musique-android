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

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override { return BoundingBox(); }

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;

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
