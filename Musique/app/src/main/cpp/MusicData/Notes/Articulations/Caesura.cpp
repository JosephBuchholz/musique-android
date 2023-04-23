#include "Caesura.h"

#include "../../../RenderMeasurement.h"

void Caesura::Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX, float offsetY) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + notePositionX + offsetY, position.y + notePositionY + offsetX, Paint(color.color)));
}

Vec2<float> Caesura::GetDimensions() const
{
    SMuFLID glyphID = GetSMuFLID();
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

SMuFLID Caesura::GetSMuFLID() const
{
    SMuFLID glyphID;

    switch (type)
    {
        case Type::Normal: glyphID = SMuFLID::caesura; break;
        case Type::Thick: glyphID = SMuFLID::caesuraThick; break;
        case Type::Short: glyphID = SMuFLID::caesuraShort; break;
        case Type::Curved: glyphID = SMuFLID::caesuraCurved; break;
        case Type::Single: glyphID = SMuFLID::caesuraSingleStroke; break;
        default: glyphID = SMuFLID::None; break;
    }
    return glyphID;
}

void Caesura::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab)
{
    position.y -= topStaffLineDistNote;
    position.y += 10.0f;

    position.x += 30.0f;
}