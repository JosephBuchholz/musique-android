/**
 * This file contains the definition of the 'Turn' class.
 */

#ifndef MUSIQUE_TURN_H
#define MUSIQUE_TURN_H

#include "Ornaments.h"

#include "../../Types.h"

/**
* This class represents a turn ornament.
 */
class Turn : public Ornament
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

    enum class TurnType {
        None = 0, Turn, DelayedTurn, InvertedTurn, DelayedInvertedTurn, VerticalTurn, InvertedVerticalTurn
    } type = TurnType::Turn;

    bool hasSlash = false;
};

#endif //MUSIQUE_TURN_H
