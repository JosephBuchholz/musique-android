#include "Stress.h"

#include "../../../RenderMeasurement.h"

void Stress::Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX, float offsetY) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + notePositionX + offsetY, position.y + notePositionY + offsetX, Paint(color.color)));
}

Vec2<float> Stress::GetDimensions() const
{
    SMuFLID glyphID = GetSMuFLID();
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

SMuFLID Stress::GetSMuFLID() const
{
    SMuFLID glyphID = SMuFLID::None;

    if (placement == AboveBelowType::Above)
    {
        switch (type)
        {
            case Type::Stress: glyphID = SMuFLID::articStressAbove; break;
            case Type::Unstress: glyphID = SMuFLID::articUnstressAbove; break;
            default: glyphID = SMuFLID::None; break;
        }
    }
    else if (placement == AboveBelowType::Below)
    {
        switch (type)
        {
            case Type::Stress: glyphID = SMuFLID::articStressBelow; break;
            case Type::Unstress: glyphID = SMuFLID::articUnstressBelow; break;
            default: glyphID = SMuFLID::None; break;
        }
    }

    return glyphID;
}

void Stress::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab)
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