#include "StaccatoArticulation.h"

#include "../../../RenderMeasurement.h"

void StaccatoArticulation::Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX, float offsetY) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + notePositionX + offsetY, position.y + notePositionY + offsetX, Paint(color.color)));
}

SMuFLID StaccatoArticulation::GetSMuFLID() const
{
    SMuFLID glyphID = SMuFLID::None;

    if (placement == AboveBelowType::Above)
    {
        switch (type)
        {
            case Type::Staccato: glyphID = SMuFLID::articStaccatoAbove; break;
            case Type::Tenuto: glyphID = SMuFLID::articTenutoAbove; break;
            case Type::DetachedLegato: glyphID = SMuFLID::articTenutoStaccatoAbove; break;
            case Type::Staccatissimo: glyphID = SMuFLID::articStaccatissimoWedgeAbove; break;
            case Type::Spiccato: glyphID = SMuFLID::articStaccatissimoAbove; break;
            default: glyphID = SMuFLID::None; break;
        }
    }
    else if (placement == AboveBelowType::Below)
    {
        switch (type)
        {
            case Type::Staccato: glyphID = SMuFLID::articStaccatoBelow; break;
            case Type::Tenuto: glyphID = SMuFLID::articTenutoBelow; break;
            case Type::DetachedLegato: glyphID = SMuFLID::articTenutoStaccatoBelow; break;
            case Type::Staccatissimo: glyphID = SMuFLID::articStaccatissimoWedgeBelow; break;
            case Type::Spiccato: glyphID = SMuFLID::articStaccatissimoBelow; break;
            default: glyphID = SMuFLID::None; break;
        }
    }

    return glyphID;
}

Vec2<float> StaccatoArticulation::GetDimensions() const
{
    SMuFLID glyphID = GetSMuFLID();
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

void StaccatoArticulation::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, float noteWidth, bool isTab)
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