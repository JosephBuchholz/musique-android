#include "Caesura.h"

#include "../../../RenderMeasurement.h"

void Caesura::Render(RenderData& renderData, Vec2<float> notePosition) const
{
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + notePosition, Paint(color.color)));
}

BoundingBox Caesura::GetBoundingBox() const
{
    BoundingBox bb;

    bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, Paint(color.color)));
    bb.position += position;

    return bb;
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

void Caesura::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY)
{
    position = { 0.0f, 0.0f };

    position.y -= topStaffLineDistNote;
    position.y += 10.0f;

    position.x += 30.0f;
}