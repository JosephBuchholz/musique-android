#include "MeasureNumber.h"

MeasureNumber::MeasureNumber(int number)
    : NumberElement(number)
{
    fontSize.size = 8.0f;
    fontStyle = FontStyle::Italic;
}

void MeasureNumber::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    // TODO: implement
}

void MeasureNumber::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    Paint paint = Paint(color.color);

    paint.textSize = fontSize.size;
    paint.isItalic = (fontStyle == FontStyle::Italic);

    renderData.AddText(Text(displayNumber, position.x + measurePosition.x + offset.x, position.y + measurePosition.y + offset.y, paint));
}

void MeasureNumber::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition)
{
    position = defaultPosition;
}