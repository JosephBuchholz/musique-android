#include "Tuplet.h"

void Tuplet::UpdateBoundingBox(const Vec2<float> &parentPosition)
{

}

void Tuplet::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    // digit
    Vec2<float> digitPosition;

    digitPosition.x = positionStart.x + ((positionEnd.x - positionStart.x) / 2.0f);
    digitPosition.y = positionStart.y + ((positionEnd.y - positionStart.y) / 2.0f);

    Paint digitPaint = Paint(color.color);

    digitPaint.centerVertically = true;
    digitPaint.centerHorizontally = true;

    digitPaint.hasBackground = true;
    digitPaint.backgroundColor = (int)0xFFFFFFFF;
    digitPaint.backgroundPadding = 3.0f;

    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), digitPosition.x + measurePosition.x + offset.x, digitPosition.y + measurePosition.y + offset.y, digitPaint));

    // bracket
    if (hasBracket)
    {
        Paint bracketPaint = Paint(color.color);

        float direction = 1.0f;
        if (placement == AboveBelowType::Below)
            direction = -1.0f;

        // bracket ends
        renderData.AddLine(std::make_shared<Line>(positionStart.x + measurePosition.x + offset.x, positionStart.y + measurePosition.y + offset.y, positionStart.x + measurePosition.x + offset.x, positionStart.y + measurePosition.y + offset.y + (endLengthHeight * direction), bracketPaint));
        renderData.AddLine(std::make_shared<Line>(positionEnd.x + measurePosition.x + offset.x, positionEnd.y + measurePosition.y + offset.y, positionEnd.x + measurePosition.x + offset.x, positionEnd.y + measurePosition.y + offset.y + (endLengthHeight * direction), bracketPaint));

        // main line
        renderData.AddLine(std::make_shared<Line>(positionStart.x + measurePosition.x + offset.x, positionStart.y + measurePosition.y + offset.y, positionEnd.x + measurePosition.x + offset.x, positionEnd.y + measurePosition.y + offset.y, bracketPaint));
    }
}

SMuFLID Tuplet::GetSMuFLID() const
{
    switch (tupletType)
    {
        case TupletType::Duplet: return SMuFLID::tuplet2;
        case TupletType::Triplet: return SMuFLID::tuplet3;
        case TupletType::Quadrulpet: return SMuFLID::tuplet4;
        case TupletType::Quintuplet: return SMuFLID::tuplet5;
        case TupletType::Sextuplet: return SMuFLID::tuplet6;
        case TupletType::Septuplet: return SMuFLID::tuplet7;
        case TupletType::Octuplet: return SMuFLID::tuplet8;
        case TupletType::Nonuplet: return SMuFLID::tuplet9;
        case TupletType::None: return SMuFLID::ErrorGlyph;
    }
}

void Tuplet::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd)
{
    if (notes.empty())
        return;

    std::shared_ptr<Note> firstNote = notes[0];
    std::shared_ptr<Note> lastNote = notes[notes.size() - 1];

    if (firstNote->noteStem->stemType == NoteStem::StemType::Up)
    {
        placement = AboveBelowType::Above;
    }
    else
    {
        placement = AboveBelowType::Below;
    }

    if (placement == AboveBelowType::Above)
    {
        positionStart = { firstNote->position.x, firstNote->position.y - 50.0f };
        positionEnd = { lastNote->position.x + lastNote->noteHead.GetNoteHeadWidth(displayConstants), lastNote->position.y - 50.0f };
    }
    else if (placement == AboveBelowType::Below)
    {
        positionStart = { firstNote->position.x, firstNote->position.y + 50.0f };
        positionEnd = { lastNote->position.x + lastNote->noteHead.GetNoteHeadWidth(displayConstants), lastNote->position.y + 50.0f };
    }
}