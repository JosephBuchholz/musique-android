#include "Accent.h"

#include "../../../RenderMeasurement.h"

void Accent::Render(RenderData& renderData, Vec2<float> notePosition) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + notePosition, Paint(color.color)));
}

BoundingBox Accent::GetBoundingBox() const
{
    BoundingBox bb;

    bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, Paint(color.color)));
    bb.position += position;

    return bb;
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

void Accent::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY)
{
    position = { 0.0f, 0.0f };

    placement = AboveBelowType::Above;

    if (isTab)
    {
        position.y -= topStaffLineDistNote;
        position.y -= 10.0f;
    }
    else // is standard
    {
        position.y = topNotePositionY;
        position.y -= 10.0f;

        if (noteStem.stemType == NoteStem::StemType::Up)
        {
            if (position.y > noteStem.stemEndY)
            {
                position.y = noteStem.stemEndY;
                position.y -= 10.0f;
            }
        }

        if (position.y > -topStaffLineDistNote - 10.0f)
        {
            position.y = -topStaffLineDistNote;
            position.y -= 10.0f;
        }



        /*if (type == Type::Accent)
        {
            if (noteStem->stemType == NoteStem::StemType::Up)
            {
                placement = AboveBelowType::Below;
            }
            else if (noteStem->stemType == NoteStem::StemType::Down)
            {
                placement = AboveBelowType::Above;
            }

            if (placement == AboveBelowType::Above)
            {
                position.y -= 10.0f;
            }
            else if (placement == AboveBelowType::Below)
            {
                position.y += 10.0f;
            }
        }
        else
        {
            placement = AboveBelowType::Above;

            if (placement == AboveBelowType::Above)
            {
                position.y -= 10.0f;
            }
            else if (placement == AboveBelowType::Below)
            {
                position.y += 10.0f;
            }
        }*/
    }

    position.x -= GetDimensions().x / 2.0f;
}

void Accent::ModifyVelocity(float& velocity)
{
    switch (type)
    {
        case Type::Accent: velocity += 20.0f; break;
        case Type::StrongAccent: velocity += 30.0f; break;
        case Type::SoftAccent: velocity -= 10.0f; break;
        default: break;
    }
}