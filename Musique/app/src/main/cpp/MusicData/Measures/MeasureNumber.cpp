#include "MeasureNumber.h"

MeasureNumber::MeasureNumber(int number)
    : NumberElement(number)
{
    fontSize.size = 8.0f;
    fontStyle = FontStyle::Italic;
}

void MeasureNumber::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    Paint paint;

    TextualElement::ModifyPaint(paint);

    BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize * 2.0f;
    bb.size.x = paint.textSize * (float)displayNumber.size();
    bb.size.y = paint.textSize * 2.0f;

    boundingBox.position.x = bb.position.x + position.x + parentPosition.x;
    boundingBox.position.y = bb.position.y + position.y + parentPosition.y;
    boundingBox.position.x -= bb.size.x / 2.0f;
    boundingBox.position.y += bb.size.y / 2.0f;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

    boundingBox.AddPadding(2.0f);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void MeasureNumber::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    Paint paint;
    TextualElement::ModifyPaint(paint);
    renderData.AddText(Text(displayNumber, position + measurePosition + offset, paint));
}

BoundingBox MeasureNumber::GetBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const
{
    Paint paint;

    TextualElement::ModifyPaint(paint);

    BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize * 2.0f;
    bb.size.x = paint.textSize * (float)displayNumber.size();
    bb.size.y = paint.textSize * 2.0f;

    BoundingBox finalBoundingBox;
    finalBoundingBox.position.x = bb.position.x + position.x;
    finalBoundingBox.position.y = bb.position.y + position.y;
    finalBoundingBox.position.x -= bb.size.x / 2.0f;
    finalBoundingBox.position.y += bb.size.y / 2.0f;
    finalBoundingBox.size.x = bb.size.x;
    finalBoundingBox.size.y = bb.size.y;

    return finalBoundingBox;
}

void MeasureNumber::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition)
{
    position = defaultPosition;
}