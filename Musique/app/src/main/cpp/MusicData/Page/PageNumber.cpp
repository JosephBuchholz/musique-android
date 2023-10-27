#include "PageNumber.h"

PageNumber::PageNumber(int number)
        : NumberElement(number)
{
    fontSize.size = 10.0f;
}

void PageNumber::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    // TODO: implement
}

void PageNumber::Render(RenderData& renderData, Vec2<float> pagePosition) const
{
    Paint paint = Paint(color.color);

    paint.textSize = fontSize.size;

    renderData.AddText(Text(displayNumber, position + pagePosition, paint));
}

void PageNumber::CalculatePosition(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition)
{
    position = defaultPosition;
}