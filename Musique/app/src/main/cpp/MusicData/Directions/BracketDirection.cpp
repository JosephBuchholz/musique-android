#include "BracketDirection.h"

#define BRACKET_DIRECTION_DEFAULT_TEXT_LINE_MARGIN 5.0f

void BracketDirectionSegment::UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX)
{
    boundingBox.size.y = 0.0f;
    boundingBox.position = { positionStart.x + startMeasurePositionX, positionStart.y + staffPositionY };
    boundingBox.size.x = (positionEnd.x + endMeasurePositionX) - (positionStart.x + startMeasurePositionX);

    boundingBox.AddPadding(10.0f);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::NoHorizontal);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void BracketDirectionSegment::Render(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, Paint paint) const
{
    VisibleElement::ModifyPaint(paint);

    LOGW("rendering segment");

    /*renderData.AddDebugDot(startMeasurePositionX, staffPositionY);
    renderData.AddDebugDot(endMeasurePositionX, staffPositionY);

    renderData.AddDebugDot(positionStart.x + startMeasurePositionX, positionStart.y + staffPositionY);
    renderData.AddDebugDot(positionEnd.x + endMeasurePositionX, positionEnd.y + staffPositionY);*/

    renderData.AddLine(Line(positionStart.x + startMeasurePositionX, positionStart.y + staffPositionY, positionEnd.x + endMeasurePositionX, positionEnd.y + staffPositionY, paint));
}

void BracketDirection::Render(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const
{
    Paint paint;

    VisibleElement::ModifyPaint(paint);
    LineElement::ModifyPaint(paint);

    LOGE("Render: staffPositionY: %f, startMeasurePositionX: %f, endMeasurePositionX: %f, startMeasureIndex: %d, endMeasureIndex: %d, segment.size(): %d, this->startMeasureIndex: %d, this->endMeasureIndex: %d",
         staffPositionY, startMeasurePositionX,
         endMeasurePositionX, startMeasureIndex, endMeasureIndex,
         segments.size(), this->startMeasureIndex, this->endMeasureIndex);

    //LOGE("Rendering dashes direction: linetype: %d, isDashed: %d, dashLength: %f, dashSpaceLength: %f", lineType, paint.isDashedLine, dashLength, dashSpaceLength);

    //renderData.AddLine(positionStart.x + measurePosition.x, positionStart.y + measurePosition.y, positionEnd.x + measurePosition.x, positionEnd.y + measurePosition.y, paint);

    for (const auto& segment : segments)
    {
        LOGD("segment: positionStart: %s | positionEnd: %s | startMeasureIndex: %d, endMeasureIndex: %d",
             segment->positionStart.GetPrintableString().c_str(), segment->positionEnd.GetPrintableString().c_str(), segment->startMeasureIndex, segment->endMeasureIndex);

        if (segment->startMeasureIndex == startMeasureIndex && segment->endMeasureIndex == endMeasureIndex)
            segment->Render(renderData, staffPositionY, startMeasurePositionX, endMeasurePositionX, paint);
    }

    if (!isDashes) // is a bracket
    {
        if (startMeasureIndex == this->startMeasureIndex)
            RenderLineEnd(renderData, { segments[0]->positionStart.x + startMeasurePositionX, segments[0]->positionStart.y + staffPositionY }, lineEndTypeStart, endLengthStart, paint);
        if (endMeasureIndex == this->endMeasureIndex)
            RenderLineEnd(renderData, { segments[segments.size() - 1]->positionEnd.x + endMeasurePositionX, segments[segments.size() - 1]->positionEnd.y + staffPositionY }, lineEndTypeStop, endLengthStop, paint);
    }

    // render text
    if (hasText)
    {
        /*paint.hasBackground = true;
        paint.backgroundColor = 0xFFFFFFFF;
        paint.backgroundPadding = 0.0f;*/

        if (startMeasureIndex == this->startMeasureIndex)
            textElement.Render(renderData, { startMeasurePositionX, staffPositionY }, paint);
    }
}

void BracketDirection::RenderDebug(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const
{
    int index = 0;
    for (const auto& segment : segments)
    {
        if (segment->startMeasureIndex == startMeasureIndex && segment->endMeasureIndex == endMeasureIndex)
        {
            //RenderSegment(renderData, segment, staffPositionY, startMeasurePositionX, endMeasurePositionX, Paint());
            //renderData.AddDebugDot(startMeasurePositionX, staffPositionY);

            BoundingBox bb = GetTotalBoundingBox(renderData.displayConstants, index, staffPositionY, startMeasurePositionX, endMeasurePositionX);

            bb.Render(renderData, 0xFFFF00FF);
        }

        index++;
    }
}

void BracketDirection::AddNewSegment(DurationDirectionSegment segment)
{
    std::shared_ptr<BracketDirectionSegment> seg = std::make_shared<BracketDirectionSegment>();
    seg->startMeasureIndex = segment.startMeasureIndex;
    seg->endMeasureIndex = segment.endMeasureIndex;
    seg->positionStart = segment.positionStart;
    seg->positionEnd = segment.positionEnd;
    seg->position = segment.position;

    // default y position
    if (defaultPositionStart.y != INVALID_FLOAT_VALUE)
    {
        seg->positionStart.y = defaultPositionStart.y;

        if (defaultPositionEnd.y != INVALID_FLOAT_VALUE)
            seg->positionEnd.y = defaultPositionEnd.y;
        else
            seg->positionEnd.y = defaultPositionStart.y;
    }

    // relative y position
    if (relativePositionStart.y != INVALID_FLOAT_VALUE)
    {
        seg->positionStart.y += relativePositionStart.y;

        if (relativePositionEnd.y != INVALID_FLOAT_VALUE)
            seg->positionEnd.y += relativePositionEnd.y;
        else
            seg->positionEnd.y += relativePositionStart.y;
    }

    segments.emplace_back(seg);
}

// TODO: this is just a quick fix (needs to be remove/modified)
void BracketDirection::Move(Vec2<float> positionOffset, Vec2<float> sizeOffset, float rotationOffset)
{
    if (hasText)
    {
        if (!segments.empty())
        {
            textElement.position.y = segments[0]->positionStart.y;
        }
    }
}

BoundingBox BracketDirection::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex, float startMeasurePositionX, float endMeasurePositionX) const
{
    BoundingBox bb;

    int index = 0;
    for (const auto& seg : segments)
    {
        if (seg->startMeasureIndex == startMeasureIndex || seg->endMeasureIndex == endMeasureIndex)
        {
            bb = GetTotalBoundingBox(displayConstants, index, 0.0f, startMeasurePositionX, endMeasurePositionX);
            break;
        }

        index++;
    }

    return bb;
}

BoundingBox BracketDirection::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int segmentIndex, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX) const
{
    BoundingBox bb;

    if (segmentIndex < segments.size())
    {
        std::shared_ptr<BracketDirectionSegment> segment = segments[segmentIndex];

        if (segment)
        {
            bb.size.y = 0.0f;
            bb.position = { segment->positionStart.x + startMeasurePositionX, segment->positionStart.y + staffPositionY };
            bb.size.x = (segment->positionEnd.x + endMeasurePositionX) - (segment->positionStart.x + startMeasurePositionX);

            bb.AddPadding(10.0f);
        }
    }

    return bb;
}

void BracketDirection::UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex)
{
    for (const auto& segment : segments)
    {
        if (segment->startMeasureIndex == startMeasureIndex && segment->endMeasureIndex == endMeasureIndex)
            segment->UpdateBoundingBox(staffPositionY, startMeasurePositionX, endMeasurePositionX);
    }

    /*boundingBox.position.x = positionStart.x + parentPosition.x;
    boundingBox.position.y = positionStart.y + parentPosition.y;
    boundingBox.size.x = positionEnd.x - positionStart.x;
    boundingBox.size.y = 2.0f;*/

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::NoHorizontal);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void BracketDirection::RenderLineEnd(RenderData& renderData, Vec2<float> position, LineEndType lineEndType, float endLength, const Paint& paint)
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

    renderData.AddLine(Line(position.x, positionLineStartY, position.x, positionLineEndY, paint));
}

void BracketDirection::CalculateAsPaged(const MusicDisplayConstants& displayConstants)
{
    //hasText = true;
    //textElement.text = "hello";

    if (hasText)
    {
        textElement.align = TextualElement::TextAlign::Left;

        float textWidth = textElement.GetDimensions(Paint()).x;
        if (!segments.empty())
        {
            textElement.position = segments[0]->positionStart;
            segments[0]->positionStart.x += textWidth;
            segments[0]->positionStart.x += BRACKET_DIRECTION_DEFAULT_TEXT_LINE_MARGIN;
        }
    }
}