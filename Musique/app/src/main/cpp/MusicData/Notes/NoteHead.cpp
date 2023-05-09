#include "NoteHead.h"

#include "../../RenderMeasurement.h"

void NoteHead::Render(RenderData& renderData, Vec2<float> position, NoteValue noteDuration, bool centerHorizontally, Vec2<float> offset) const
{
    if (centerHorizontally)
        position.x -= GetDimensions(noteDuration).x / 2.0f;

    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(noteDuration), position.x + offset.x, position.y + offset.y, Paint(color)));
}

Vec2<float> NoteHead::GetDimensions(NoteValue noteDuration) const
{
    SMuFLID glyphID = GetSMuFLID(noteDuration);
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

SMuFLID NoteHead::GetSMuFLID(NoteValue noteDuration) const
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