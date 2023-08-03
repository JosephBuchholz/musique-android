#include "NoteFlag.h"

void NoteFlag::Render(RenderData& renderData, Vec2<float> endOfNoteStemPosition, Vec2<float> offset) const
{
    if (type != NoteFlag::Type::None)
    {
        Paint paint;
        VisibleElement::ModifyPaint(paint);

        paint.glyphSizeFactor = size;

        renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), { endOfNoteStemPosition.x + position.x + offset.x, endOfNoteStemPosition.y + position.y + offset.y }, paint));
    }
}

SMuFLID NoteFlag::GetSMuFLID() const
{
    if (type != NoteFlag::Type::None)
    {
        switch (noteDuration)
        {
            case NoteValue::Eighth: if (type == Type::Up) return SMuFLID::flag8thUp; else return SMuFLID::flag8thDown;
            case NoteValue::Sixteenth: if (type == Type::Up) return SMuFLID::flag16thUp; else return SMuFLID::flag16thDown;
            case NoteValue::ThirtySecond: if (type == Type::Up) return SMuFLID::flag32ndUp; else return SMuFLID::flag32ndDown;
            default: return SMuFLID::ErrorGlyph;
        }
    }

    return SMuFLID::None;
}

void NoteFlag::CalculateAsPaged(const MusicDisplayConstants& displayConstants, float defaultSize)
{
    position = { 0.0f, 0.0f };
    size = defaultSize;
}