/**
 * This file contains the definition for the 'TrillMark' class.
 */

#ifndef MUSIQUE_TRILLMARK_H
#define MUSIQUE_TRILLMARK_H

#include "Ornaments.h"

#include "../../Types.h"

#include "../../ComplexLine.h"

/**
* This class represents a trill mark.
 */
class TrillMark : public Ornament
{
public:

    TrillMark()
    {
        wavyLine = std::make_shared<ComplexLine>();
    }

    void Render(RenderData& renderData, float notePositionX, float topStaffLinePositionY, Vec2<float> offset = { 0.0f, 0.0f }) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;
    Paint GetPaint() const;

    void UpdateBoundingBox(const Vec2<float>& parentPosition) override;

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition) override;

public:

    std::shared_ptr<ComplexLine> wavyLine;

    // -- Playback --

    // whether the trill accelerates during playback
    bool accelerate = false;

    // the number of distinct notes during playback
    unsigned int beats = 3;

    OrnamentStartNote startNote = OrnamentStartNote::Main;
    OrnamentStep trillStep = OrnamentStep::Whole;
    OrnamentTwoNoteTurn twoNoteTurn = OrnamentTwoNoteTurn::None;
};

#endif //MUSIQUE_TRILLMARK_H
