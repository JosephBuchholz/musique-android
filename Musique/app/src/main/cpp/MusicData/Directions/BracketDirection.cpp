#include "BracketDirection.h"

void BracketDirection::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    Paint paint = Paint(color.color);

    ModifyPaint(paint);

    //LOGE("Rendering dashes direction: linetype: %d, isDashed: %d, dashLength: %f, dashSpaceLength: %f", lineType, paint.isDashedLine, dashLength, dashSpaceLength);

    renderData.AddLine(positionStart.x + measurePosition.x + offset.x, positionStart.y + measurePosition.y + offset.y, positionEnd.x + measurePosition.x + offset.x, positionEnd.y + measurePosition.y + offset.y, paint);

    if (!isDashes) // is a bracket
    {
        RenderLineEnd(renderData, positionStart + measurePosition, lineEndTypeStart, endLengthStart, paint, offset);
        RenderLineEnd(renderData, positionEnd + measurePosition, lineEndTypeStop, endLengthStop, paint, offset);
    }
}

void BracketDirection::RenderLineEnd(RenderData& renderData, Vec2<float> position, LineEndType lineEndType, float endLength, const Paint& paint, Vec2<float> offset)
{
    float positionLineStartY = position.y;
    float positionLineEndY = position.y;

    if (lineEndType == LineEndType::Up)
    {
        positionLineStartY = position.y - endLength;
        positionLineEndY = position.y;
    }
    else if (lineEndType == LineEndType::Down)
    {
        positionLineStartY = position.y;
        positionLineEndY = position.y + endLength;
    }
    else if (lineEndType == LineEndType::Both)
    {
        positionLineStartY = position.y - endLength;
        positionLineEndY = position.y + endLength;
    }
    else if (lineEndType == LineEndType::Arrow)
    {
        LOGW("Can't render arrow line-ends yet");
        return;
    }
    else if (lineEndType == LineEndType::NoEnd || lineEndType == LineEndType::None)
    {
        return;
    }

    renderData.AddLine(position.x + offset.x, positionLineStartY + offset.y, position.x + offset.x, positionLineEndY + offset.y, paint);
}

void BracketDirection::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd)
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
}