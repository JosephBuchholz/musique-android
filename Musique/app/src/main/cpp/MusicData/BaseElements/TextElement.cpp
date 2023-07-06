#include "TextElement.h"

#include "../../RenderMeasurement.h"

void TextElement::Render(RenderData& renderData, Vec2<float> parentPosition, Paint parentPaint) const
{
    TextualElement::ModifyPaint(parentPaint);

    renderData.AddText(Text(text, position.x + parentPosition.x, position.y + parentPosition.y, parentPaint));
}

BoundingBox TextElement::GetBoundingBox(Paint parentPaint) const
{
    TextualElement::ModifyPaint(parentPaint);

    BoundingBox bb = RenderMeasurement::GetTextBoundingBox(Text(text, 0.0f, 0.0f, parentPaint));

    return bb;
}

Vec2<float> TextElement::GetDimensions(Paint parentPaint) const
{
    return GetBoundingBox(parentPaint).size;
}