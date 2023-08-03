#include "Ending.h"

void Ending::Render(RenderData& renderData, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, int startIndex, int endIndex) const
{
    for (const auto& segment : segments)
    {
        if (segment->startMeasureIndex == startIndex && segment->endMeasureIndex == endIndex)
            RenderSegment(renderData, segment, measureStartPosition, measureEndPosition, Paint());
    }

    // render lines

    /*LOGE("Rendering ending: measureStartPos: %s, measureEndPos: %s, startMeasureIndex: %d, endMeasureIndex: %d", measureStartPosition.GetPrintableString().c_str(), measureEndPosition.GetPrintableString().c_str(), startMeasureIndex, endMeasureIndex);

    Paint linePaint;
    VisibleElement::ModifyPaint(linePaint);
    LineElement::ModifyPaint(linePaint);

    //Vec2<float> startPosition = { position.x + measurePosition.x + offset.x, position.y + measurePosition.y + offset.y };
    //Vec2<float> endPosition = { startPosition.x + measureWidth, startPosition.y };

    Vec2<float> startPosition = { position.x + measureStartPosition.x + offset.x, position.y + measureStartPosition.y + offset.y };
    Vec2<float> endPosition = { position.x + measureEndPosition.x + offset.x, position.y + measureEndPosition.y + offset.y };

    // top line
    renderData.AddLine(startPosition.x, startPosition.y, endPosition.x, endPosition.y, linePaint);

    // front jog
    renderData.AddLine(startPosition.x, startPosition.y, startPosition.x, startPosition.y + jogLength, linePaint);

    // final jog
    if (hasFinalDownwardJog)
        renderData.AddLine(endPosition.x, endPosition.y, endPosition.x, endPosition.y + jogLength, linePaint);

    // render text

    Paint textPaint;
    TextualElement::ModifyPaint(textPaint);
    textPaint.align = Paint::Align::Left;

    renderData.AddText(Text(endingNumbersText, textPosition.x + startPosition.x, textPosition.y + startPosition.y, textPaint));*/
}

void Ending::RenderSegment(RenderData& renderData, std::shared_ptr<EndingSegment> segment, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, Paint paint) const
{
    Paint linePaint;
    VisibleElement::ModifyPaint(linePaint);
    LineElement::ModifyPaint(linePaint);

    Vec2<float> startPosition = { segment->positionStart.x + measureStartPosition.x, segment->positionStart.y + measureStartPosition.y };
    Vec2<float> endPosition = { segment->positionEnd.x + measureEndPosition.x, segment->positionEnd.y + measureEndPosition.y };

    // top line
    renderData.AddLine(Line(startPosition.x, startPosition.y, endPosition.x, endPosition.y, linePaint));

    if (segment->isEnd)
    {
        // final jog
        if (hasFinalDownwardJog)
            renderData.AddLine(Line(endPosition.x, endPosition.y, endPosition.x, endPosition.y + jogLength, linePaint));
    }

    if (segment->isStart)
    {
        // front jog
        renderData.AddLine(Line(startPosition.x, startPosition.y, startPosition.x, startPosition.y + jogLength, linePaint));

        // render text

        Paint textPaint;
        TextualElement::ModifyPaint(textPaint);
        textPaint.align = Paint::Align::Left;

        renderData.AddText(Text(endingNumbersText, textPosition.x + startPosition.x, textPosition.y + startPosition.y, textPaint));
    }
}

BoundingBox Ending::GetBoundingBoxRelativeToParent() const
{
    BoundingBox bb;

    bb.position = position;
    bb.size.x = 10.0f; // TODO: need to get width??
    bb.size.y = jogLength;

    return bb;
}

void Ending::UpdateBoundingBox(Vec2<float> parentPosition)
{
    boundingBox = GetBoundingBoxRelativeToParent();
    boundingBox.position += parentPosition;

    boundingBox.AddPadding();

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::NoHorizontal);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void Ending::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{
    if (defaultPosition.y != INVALID_FLOAT_VALUE)
    {
        position.y = -defaultPosition.y;
    }
    else
    {
        position.y = -30.0f;
    }

    for (auto segment : segments)
    {
        segment->positionStart.y = position.y;
        segment->positionEnd.y = position.y;
    }

    textPosition = { 5.0f, 10.0f };
}