#include "BreathMark.h"

#include "../../../RenderMeasurement.h"

void BreathMark::Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX, float offsetY) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + notePositionX + offsetY, position.y + notePositionY + offsetX, Paint(color.color)));
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

void BreathMark::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab)
{
    position.y -= topStaffLineDistNote;
    position.y -= 10.0f;

    position.x += 30.0f;
}