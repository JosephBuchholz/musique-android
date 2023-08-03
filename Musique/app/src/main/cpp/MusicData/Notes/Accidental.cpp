#include "Accidental.h"

#include "../../RenderMeasurement.h"

void Accidental::Render(RenderData& renderData, Vec2<float> parentPosition, Vec2<float> offset) const
{
    if (accidentalType != AccidentalType::None)
        renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + parentPosition + offset, GetPaint()));
}

Vec2<float> Accidental::GetDimensions() const
{
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, GetPaint()));
    return { bb.size.x, bb.size.y };
}

SMuFLID Accidental::GetSMuFLID() const
{
    switch (accidentalType)
    {
        case Accidental::AccidentalType::None: return SMuFLID::None;
        case Accidental::AccidentalType::Sharp: return SMuFLID::accidentalSharp;
        case Accidental::AccidentalType::Flat: return SMuFLID::accidentalFlat;
        case Accidental::AccidentalType::Natural: return SMuFLID::accidentalNatural;
        case Accidental::AccidentalType::DoubleSharp: return SMuFLID::accidentalDoubleSharp;
        case Accidental::AccidentalType::DoubleFlat: return SMuFLID::accidentalDoubleFlat;
    }

    return SMuFLID::None;
}

Paint Accidental::GetPaint() const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    paint.glyphSizeFactor = size;
    return paint;
}

void Accidental::CalculateAsPaged(const MusicDisplayConstants& displayConstants, NoteSize noteSize)
{
    if (noteSize == NoteSize::Grace)
        size = displayConstants.graceNoteSize;
    else if (noteSize == NoteSize::Cue)
        size = displayConstants.cueNoteSize;
    else
        size = 1.0f;

    position.x -= GetDimensions().x + (3.5f * size);
}