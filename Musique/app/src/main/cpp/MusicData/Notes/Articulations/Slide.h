/**
 * This files defines the Slide class.
 */

#ifndef MUSIQUE_SLIDE_H
#define MUSIQUE_SLIDE_H

#include "Articulation.h"

/**
 * This class represents the slide articulations (such as scoop or doit).
 */
class Slide : public Articulation
{
public:

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override;

    Vec2<float> GetDimensions() const override;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;

public:
    enum class Type {
        None = 0, Scoop, Plop, Doit, Falloff
    } type = Type::None;

    // the start and end points for the slide line (the positions are relative to the main articulation position)
    Vec2<float> lineStartPosition = { 0.0f, 0.0f };
    Vec2<float> lineEndPosition = { 0.0f, 0.0f };
};

#endif //MUSIQUE_SLIDE_H
