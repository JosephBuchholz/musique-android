#include "BreathMark.h"

#include "../../../RenderMeasurement.h"

void BreathMark::Render(RenderData& renderData, Vec2<float> notePosition) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + notePosition.x, position.y + notePosition.y, Paint(color.color)));
}

BoundingBox BreathMark::GetBoundingBox() const
{
    BoundingBox bb;

    bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), 0.0f, 0.0f, Paint(color.color)));
    bb.position += position;

    return bb;
}

Vec2<float> BreathMark::GetDimensions() const
{
    SMuFLID glyphID = GetSMuFLID();
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

SMuFLID BreathMark::GetSMuFLID() const
{
    return SMuFLID::breathMarkComma;
}

void BreathMark::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, std::shared_ptr<NoteStem> noteStem, float topNotePositionY, float bottomNotePositionY)
{
    position = { 0.0f, 0.0f };

    position.y -= topStaffLineDistNote;
    position.y -= 10.0f;

    position.x += 30.0f;
}