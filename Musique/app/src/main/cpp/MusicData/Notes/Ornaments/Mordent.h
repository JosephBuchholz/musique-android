/**
 * This file contains the definition of the 'Mordent' class.
 */

#ifndef MUSIQUE_MORDENT_H
#define MUSIQUE_MORDENT_H

#include "Ornaments.h"

#include "../../Types.h"

/**
* This class represents a mordent ornament.
 */
class Mordent : public Ornament
{
public:

    void Render(RenderData& renderData, float notePositionX, float topStaffLinePositionY, Vec2<float> offset = { 0.0f, 0.0f }) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;
    Paint GetPaint() const;

    void UpdateBoundingBox(const Vec2<float>& parentPosition) override;

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition) override;

public:

    enum class MordentType {
        None = 0, Mordent, InvertedMordent
    } type = MordentType::Mordent;

    // -- Playback --

    OrnamentStartNote startNote = OrnamentStartNote::Main;
    OrnamentStep trillStep = OrnamentStep::Whole;
    OrnamentTwoNoteTurn twoNoteTurn = OrnamentTwoNoteTurn::None;
};

#endif //MUSIQUE_MORDENT_H
