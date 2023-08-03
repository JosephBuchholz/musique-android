#include "GuitarTechnique.h"

#include "../../../RenderMeasurement.h"

void GuitarTechnique::Render(RenderData& renderData, Vec2<float> notePosition) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + notePosition, Paint(color.color)));
}

Vec2<float> GuitarTechnique::GetDimensions() const
{
    SMuFLID glyphID = GetSMuFLID();
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

SMuFLID GuitarTechnique::GetSMuFLID() const
{
    switch (type)
    {
        case Type::Tap:
        {
            if (hand == Hand::Right)
                return SMuFLID::guitarRightHandTapping;
            else if (hand == Hand::Left)
                return SMuFLID::guitarLeftHandTapping;
            else
                return SMuFLID::ErrorGlyph;
        }
        case Type::Golpe: return SMuFLID::guitarGolpe;
        default: return SMuFLID::ErrorGlyph;
    }
}

void GuitarTechnique::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY)
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

    position.x -= GetDimensions().x / 2.0f;
}