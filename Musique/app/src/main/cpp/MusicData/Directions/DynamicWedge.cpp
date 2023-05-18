#include "DynamicWedge.h"

void DynamicWedge::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    boundingBox.size.x = positionEnd.x - positionStart.x;
    boundingBox.size.y = spread;

    boundingBox.position.x = positionStart.x + parentPosition.x;
    boundingBox.position.y = positionStart.y - (boundingBox.size.y / 2.0f) + parentPosition.y;

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void DynamicWedge::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    //LOGE("Rendering dynamic wedge: sPos.x: %f | sPos.y: %f | ePos.x: %f | ePos.y: %f", positionStart.x, positionStart.y, positionEnd.x, positionEnd.y);

    Paint paint;

    VisibleElement::ModifyPaint(paint);
    LineElement::ModifyPaint(paint);

    if (type == WedgeType::Crescendo)
    {
        renderData.AddLine(positionStart.x + measurePosition.x + offset.x, positionStart.y + measurePosition.y + offset.y, positionEnd.x + measurePosition.x + offset.x, positionEnd.y + measurePosition.y + offset.y + (spread/2.0f), paint); // bottom line
        renderData.AddLine(positionStart.x + measurePosition.x + offset.x, positionStart.y + measurePosition.y + offset.y, positionEnd.x + measurePosition.x + offset.x, positionEnd.y + measurePosition.y + offset.y - (spread/2.0f), paint); // top line
    }
    else if (type == WedgeType::Diminuendo)
    {
        renderData.AddLine(positionStart.x + measurePosition.x + offset.x, positionStart.y + measurePosition.y + offset.y + (spread/2.0f), positionEnd.x + measurePosition.x + offset.x, positionEnd.y + measurePosition.y + offset.y, paint); // bottom line
        renderData.AddLine(positionStart.x + measurePosition.x + offset.x, positionStart.y + measurePosition.y + offset.y - (spread/2.0f), positionEnd.x + measurePosition.x + offset.x, positionEnd.y + measurePosition.y + offset.y, paint); // top line
    }
}

void DynamicWedge::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd, float defSpread)
{
    positionStart = defPositionStart;
    positionEnd = defPositionEnd;

    // default y position
    if (defaultPositionStart.y != INVALID_FLOAT_VALUE)
    {
        positionStart.y = defaultPositionStart.y;

        if (defaultPositionEnd.y != INVALID_FLOAT_VALUE)
            positionEnd.y = defaultPositionEnd.y;
        else
            positionEnd.y = defaultPositionStart.y;
    }

    // relative y position
    if (relativePositionStart.y != INVALID_FLOAT_VALUE)
    {
        positionStart.y += relativePositionStart.y;

        if (relativePositionEnd.y != INVALID_FLOAT_VALUE)
            positionEnd.y += relativePositionEnd.y;
        else
            positionEnd.y += relativePositionStart.y;
    }

    // spread
    if (defaultSpread != INVALID_FLOAT_VALUE)
        spread = defaultSpread;
    else
        spread = defSpread;
}