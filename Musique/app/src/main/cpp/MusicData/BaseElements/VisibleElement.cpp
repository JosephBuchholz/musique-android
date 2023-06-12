#include "VisibleElement.h"

void VisibleElement::ModifyPaint(Paint& paint) const
{
    paint.color = color.color;
    paint.glyphSizeFactor = sizeFactor;
}

void VisibleElement::RenderDebug(RenderData& renderData) const
{
#if DEBUG_BOUNDING_BOXES
    debugBoundingBox.Render(renderData, 0xFF00FF00);
#endif

    boundingBox.Render(renderData, 0xFF00FFFF);
}

void VisibleElement::Move(Vec2<float> positionOffset, Vec2<float> sizeOffset, float rotationOffset)
{
    position += positionOffset;
    rotation += rotationOffset;
}