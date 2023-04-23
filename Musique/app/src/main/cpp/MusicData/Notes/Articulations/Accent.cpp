#include "Accent.h"

#include "../../../RenderMeasurement.h"

void Accent::Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX, float offsetY) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + notePositionX + offsetY, position.y + notePositionY + offsetX, Paint(color.color)));
}

Vec2<float> Accent::GetDimensions() const
{
    SMuFLID glyphID = GetSMuFLID();
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

SMuFLID Accent::GetSMuFLID() const
{
    SMuFLID glyphID = SMuFLID::None;

    if (placement == AboveBelowType::Above)
    {
        switch (type)
        {
            case Type::Accent: glyphID = SMuFLID::articAccentAbove; break;
            case Type::StrongAccent: glyphID = SMuFLID::articMarcatoAbove; break;
            case Type::SoftAccent: glyphID = SMuFLID::ErrorGlyph; break;
            default: glyphID = SMuFLID::None; break;
        }
    }
    else if (placement == AboveBelowType::Below)
    {
        switch (type)
        {
            case Type::Accent: glyphID = SMuFLID::articAccentBelow; break;
            case Type::StrongAccent: glyphID = SMuFLID::articMarcatoBelow; break;
            case Type::SoftAccent: glyphID = SMuFLID::ErrorGlyph; break;
            default: glyphID = SMuFLID::None; break;
        }
    }

    return glyphID;
}

void Accent::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab)
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