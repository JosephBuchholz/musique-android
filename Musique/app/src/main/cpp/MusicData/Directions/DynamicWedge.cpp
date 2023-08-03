#include "DynamicWedge.h"

#define DYNAMIC_WEDGE_DEFAULT_SPREAD 15.0f
#define DYNAMIC_WEDGE_DEFAULT_CONTINUE_SPREAD 5.0f
#define DYNAMIC_WEDGE_DEFAULT_MARGIN 4.0f

void DynamicWedgeSegment::UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX)
{
    boundingBox.size.y = DYNAMIC_WEDGE_DEFAULT_SPREAD;
    boundingBox.position = { positionStart.x + startMeasurePositionX, positionStart.y + staffPositionY };
    boundingBox.size.x = (positionEnd.x + endMeasurePositionX) - (positionStart.x + startMeasurePositionX);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::NoHorizontal);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void DynamicWedge::Render(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const
{
    for (const auto& segment : segments)
    {
        //LOGD("segment: positionStart: %s | positionEnd: %s | startMeasureIndex: %d, endMeasureIndex: %d",
        //     segment.positionStart.GetPrintableString().c_str(), segment.positionEnd.GetPrintableString().c_str(), segment.startMeasureIndex, segment.endMeasureIndex);

        if (segment->startMeasureIndex == startMeasureIndex && segment->endMeasureIndex == endMeasureIndex)
            RenderSegment(renderData, segment, staffPositionY, startMeasurePositionX, endMeasurePositionX, Paint());
    }
}

void DynamicWedge::RenderDebug(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const
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

void DynamicWedge::RenderSegment(RenderData& renderData, std::shared_ptr<DynamicWedgeSegment> segment, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, Paint paint) const
{
    RenderWedge(renderData, segment->positionStart + Vec2<float>{ startMeasurePositionX, staffPositionY }, segment->positionEnd + Vec2<float>{ endMeasurePositionX, staffPositionY }, segment->rightSideIsOpen, segment->leftSideIsOpen);
}

void DynamicWedge::AddNewSegment(DurationDirectionSegment segment)
{
    std::shared_ptr<DynamicWedgeSegment> seg = std::make_shared<DynamicWedgeSegment>();
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

    segments.push_back(seg);
}

void DynamicWedge::RenderWedge(RenderData& renderData, Vec2<float> startPosition, Vec2<float> endPosition, bool rightSideIsOpen, bool leftSideIsOpen) const
{
    Paint paint;

    VisibleElement::ModifyPaint(paint);
    LineElement::ModifyPaint(paint);

    float otherSpread = 0.0f;
    if (type == WedgeType::Crescendo)
    {
        if (leftSideIsOpen)
            otherSpread = DYNAMIC_WEDGE_DEFAULT_CONTINUE_SPREAD;

        renderData.AddLine(Line(startPosition.x, startPosition.y + (otherSpread/2.0f), endPosition.x, endPosition.y + (spread/2.0f), paint)); // bottom line
        renderData.AddLine(Line(startPosition.x, startPosition.y - (otherSpread/2.0f), endPosition.x, endPosition.y - (spread/2.0f), paint)); // top line
    }
    else if (type == WedgeType::Diminuendo)
    {
        if (rightSideIsOpen)
            otherSpread = DYNAMIC_WEDGE_DEFAULT_CONTINUE_SPREAD;

        renderData.AddLine(Line(startPosition.x, startPosition.y - (spread/2.0f), endPosition.x, endPosition.y - (otherSpread/2.0f), paint)); // top line
        renderData.AddLine(Line(startPosition.x, startPosition.y + (spread/2.0f), endPosition.x, endPosition.y + (otherSpread/2.0f), paint)); // bottom line
    }
}

void DynamicWedge::CalculateAsPaged(const MusicDisplayConstants& displayConstants)
{
    position = { 0.0f, 0.0f };

    // spread
    if (defaultSpread != INVALID_FLOAT_VALUE)
        spread = defaultSpread;
    else
        spread = DYNAMIC_WEDGE_DEFAULT_SPREAD;

    if (segments.size() > 1)
    {
        segments[0]->rightSideIsOpen = true;
        segments[segments.size() - 1]->leftSideIsOpen = true;

        for (int i = 1; i < segments.size() - 1; i++) // leave out first and last segments
        {
            segments[i]->leftSideIsOpen = true;
            segments[i]->rightSideIsOpen = true;
        }
    }

    for (auto& segment : segments)
    {
        segment->positionStart.x += DYNAMIC_WEDGE_DEFAULT_MARGIN;
        segment->positionEnd.x -= DYNAMIC_WEDGE_DEFAULT_MARGIN;
    }
}

BoundingBox DynamicWedge::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex, float startMeasurePositionX, float endMeasurePositionX) const
{
    BoundingBox bb;

    int index = 0;
    for (auto seg : segments)
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

BoundingBox DynamicWedge::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int segmentIndex, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX) const
{
    BoundingBox bb;

    if (segmentIndex < segments.size())
    {
        std::shared_ptr<DynamicWedgeSegment> segment = segments[segmentIndex];

        if (segment)
        {
            bb.size.x = (segment->positionEnd.x + endMeasurePositionX) - (segment->positionStart.x + startMeasurePositionX);
            bb.size.y = spread;
            bb.position = { segment->positionStart.x + startMeasurePositionX, segment->positionStart.y + staffPositionY - (bb.size.y / 2.0f) };
        }
    }

    return bb;
}

void DynamicWedge::UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex)
{
    for (const auto& segment : segments)
    {
        if (segment->startMeasureIndex == startMeasureIndex && segment->endMeasureIndex == endMeasureIndex)
            segment->UpdateBoundingBox(staffPositionY, startMeasurePositionX, endMeasurePositionX);
    }

    /*boundingBox.size.x = positionEnd.x - positionStart.x;
    boundingBox.size.y = spread;

    boundingBox.position.x = positionStart.x + parentPosition.x;
    boundingBox.position.y = positionStart.y - (boundingBox.size.y / 2.0f) + parentPosition.y;*/

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::None);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}
