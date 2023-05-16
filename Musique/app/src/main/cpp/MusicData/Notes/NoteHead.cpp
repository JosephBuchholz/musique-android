#include "NoteHead.h"

#include "../../RenderMeasurement.h"

void NoteHead::Render(RenderData& renderData, Vec2<float> position, bool centerHorizontally, Vec2<float> offset) const
{
    if (noteType == NoteType::Standard)
    {
        if (centerHorizontally)
            position.x -= GetDimensions(renderData.displayConstants).x / 2.0f;

        renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + offset.x, position.y + offset.y, GetPaint(renderData.displayConstants)));
    }
    else if (noteType == NoteType::Tab)
    {
        if (type == NoteHeadType::X)
        {
            position.x -= GetDimensions(renderData.displayConstants).x / 2.0f;
            renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + offset.x, position.y + offset.y, GetPaint(renderData.displayConstants)));
        }
        else
            renderData.AddText(Text(ToString(fret), position.x + offset.x, position.y + offset.y, GetPaint(renderData.displayConstants)));
    }
}

Vec2<float> NoteHead::GetDimensions(const MusicDisplayConstants& displayConstants) const
{
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), 0.0f, 0.0f, GetPaint(displayConstants)));
    Paint paint = GetPaint(displayConstants);

    if (noteType == NoteType::Standard)
        bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), 0.0f, 0.0f, paint));
    else if (noteType == NoteType::Tab)
        bb = RenderMeasurement::GetTextBoundingBox(Text(ToString(fret), 0.0f, 0.0f, paint));

    return { bb.size.x, bb.size.y };
}

SMuFLID NoteHead::GetSMuFLID() const
{
    switch (type)
    {
        case NoteHeadType::Normal:
        {
            switch (noteDuration)
            {
                case NoteValue::None: return SMuFLID::None;
                case NoteValue::Whole: return SMuFLID::noteheadWhole;
                case NoteValue::Half: return SMuFLID::noteheadHalf;

                case NoteValue::Quarter:
                case NoteValue::Eighth:
                case NoteValue::Sixteenth:
                case NoteValue::ThirtySecond: return SMuFLID::noteheadBlack;

                default: return SMuFLID::noteheadDoubleWhole;
            }
        }
        case NoteHeadType::Diamond: return SMuFLID::noteheadDiamondBlack;
        case NoteHeadType::Slash: return SMuFLID::noteheadSlashHorizontalEnds;
        case NoteHeadType::X: return SMuFLID::noteheadXBlack;
        case NoteHeadType::NoNoteHead:
        default: return SMuFLID::None;
    }
}

Paint NoteHead::GetPaint(const MusicDisplayConstants& displayConstants) const
{
    Paint paint;

    if (noteType == NoteType::Tab)
    {
        paint.textSize = 16.0f * size;
        paint.isTablature = true;
        paint.align = Paint::Align::Center;
    }

    VisibleElement::ModifyPaint(paint);
    paint.glyphSizeFactor = size;

    return paint;
}

float NoteHead::GetNoteHeadWidth(const MusicDisplayConstants& displayConstants) const // TODO: get actual width for tab
{
    return GetDimensions(displayConstants).x;
}

void NoteHead::CalculateAsPaged(const MusicDisplayConstants& displayConstants, NoteSize noteSize)
{
    if (noteSize == NoteSize::Grace)
        size = displayConstants.graceNoteSize;
    else if (noteSize == NoteSize::Cue)
        size = displayConstants.cueNoteSize;
    else
        size = 1.0f;
}