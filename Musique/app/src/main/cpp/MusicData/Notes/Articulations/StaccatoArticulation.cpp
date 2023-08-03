#include "StaccatoArticulation.h"

#include "../../../RenderMeasurement.h"

void StaccatoArticulation::Render(RenderData& renderData, Vec2<float> notePosition) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + notePosition, Paint(color.color)));
}

BoundingBox StaccatoArticulation::GetBoundingBox() const
{
    BoundingBox bb;

    bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, Paint(color.color)));
    bb.position += position;

    return bb;
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

void StaccatoArticulation::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY)
{
    position = { 0.0f, 0.0f };

    if (isTab)
    {
        position.y -= topStaffLineDistNote;
        position.y -= 10.0f;
    }
    else // is standard
    {
        if (noteStem.stemType == NoteStem::StemType::Up)
        {
            placement = AboveBelowType::Below;
        }
        else if (noteStem.stemType == NoteStem::StemType::Down)
        {
            placement = AboveBelowType::Above;
        }

        if (placement == AboveBelowType::Above)
        {
            position.y = topNotePositionY;
            position.y -= 10.0f;
        }
        else if (placement == AboveBelowType::Below)
        {
            position.y = bottomNotePositionY;
            position.y += 10.0f;
        }
    }

    position.x -= GetDimensions().x / 2.0f;
}

void StaccatoArticulation::ModifySoundDuration(float& soundDuration)
{
    switch (type)
    {
        case Type::Staccato: soundDuration /= 2.0f; break;
        case Type::Staccatissimo: soundDuration /= 3.0f; break;
        case Type::Spiccato: soundDuration /= 4.0f; break;

        case Type::Tenuto:
        case Type::DetachedLegato:
        default: break;
    }
}

void StaccatoArticulation::ModifyVelocity(float& velocity)
{
    switch (type)
    {
        case Type::Tenuto: velocity += 10.0f; break;
        case Type::DetachedLegato: velocity += 10.0f; break;
        default: break;
    }
}