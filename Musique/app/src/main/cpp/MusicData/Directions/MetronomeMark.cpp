#include "MetronomeMark.h"

void MetronomeMark::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    std::string string = " = " + tempo;

    std::vector<uint16_t> chars;
    chars.push_back((uint16_t)GetMetronomeNoteSMuFLID(mainNoteUnit.noteValue));
    if (mainNoteUnit.isDotted)
        chars.push_back((uint16_t)SMuFLID::metAugmentationDot);

    for (auto& c : string)
    {
        if (c == '\0')
            break;
        chars.push_back(c);
    }

    std::vector<TextSpan> spans;
    Paint glyphPaint = Paint(color.color);
    glyphPaint.useMusicFont = true;
    glyphPaint.textSize = 40.0f;
    glyphPaint.align = Paint::Align::Left;

    Paint paint = Paint(color.color);
    paint.isBold = true;
    paint.textSize = 26.0f;
    paint.align = Paint::Align::Left;

    if (mainNoteUnit.isDotted)
    {
        spans.emplace_back(0, 2, glyphPaint);
        spans.emplace_back(2, 10, paint);
    }
    else
    {
        spans.emplace_back(0, 1, glyphPaint);
        spans.emplace_back(1, 10, paint);
    }

    renderData.AddSpannableText(SpannableText(chars, { position.x + measurePosition.x + offset.x, position.y + measurePosition.y + offset.y }, spans, Paint(color.color)));
    //renderData.AddDebugDot(positionX + measurePosition.x + offset.x, positionY + measurePosition.y + offset.y);
}

Vec2<float> MetronomeMark::GetDimensions() const
{
    Paint paint = GetPaint();

    return { paint.textSize * ((float)tempo.size() + 5), paint.textSize * 3.0f }; // TODO: temp
}

Paint MetronomeMark::GetPaint() const
{
    Paint paint = Paint();
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    return paint;
}

void MetronomeMark::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    Paint paint = GetPaint();

    BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize;
    bb.size.x = paint.textSize * ((float)tempo.size() + 5);
    bb.size.y = paint.textSize * 3.0f;

    boundingBox.position.x = bb.position.x + position.x + parentPosition.x;
    boundingBox.position.y = bb.position.y + position.y + parentPosition.y;
    boundingBox.position.y -= bb.size.y / 2.0f;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::None);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void MetronomeMark::Move(Vec2<float> positionOffset, Vec2<float> sizeOffset, float rotationOffset)
{
    position += positionOffset;
}

void MetronomeMark::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY)
{
    if (noDefX)
        position.x = defaultX + relX;
    else
        position.x = defX + relX;

    if (noDefY)
        position.y = defaultY;
    else
        position.y = -defY + -relY;

    position.x = defaultX - (GetDimensions().x / 2.0f);
}