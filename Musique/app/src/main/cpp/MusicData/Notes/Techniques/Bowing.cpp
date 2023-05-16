#include "Bowing.h"

#include "../../../RenderMeasurement.h"

void Bowing::Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX, float offsetY) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + notePositionX + offsetX, position.y + notePositionY + offsetY, Paint(color.color)));
}

Vec2<float> Bowing::GetDimensions() const
{
    SMuFLID glyphID = GetSMuFLID();
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

SMuFLID Bowing::GetSMuFLID() const
{
    switch (direction)
    {
        case Direction::Up: return SMuFLID::stringsUpBow;
        case Direction::Down: return SMuFLID::stringsDownBow;
        default: return SMuFLID::ErrorGlyph;
    }
}

void Bowing::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab)
{
    if (isTab)
    {
        position.y -= topStaffLineDistNote;
        position.y -= 10.0f;
    }
    else // is standard
    {
        if (placement == AboveBelowType::Above)
        {
            position.y -= 10.0f;
        }
        else if (placement == AboveBelowType::Below)
        {
            position.y += 10.0f;
        }
    }

    position.x += noteWidth / 2.0f;
    position.x -= GetDimensions().x / 2.0f;
}