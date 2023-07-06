#include "Direction.h"

void Direction::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    if (!rehearsals.empty())
    {
        for (const auto& rehearsal : rehearsals)
        {
            rehearsal.Render(renderData, measurePosition + offset);
        }
    }
    else if (!words.empty())
    {
        for (const auto& word : words)
        {
            word.Render(renderData, measurePosition + offset);
        }
    }
    else if (metronomeMark != nullptr)
    {
        metronomeMark->Render(renderData, measurePosition + offset);
    }
    else if (!dynamics.empty())
    {
        for (const Dynamic& dynamic : dynamics)
        {
            dynamic.Render(renderData, measurePosition + offset);
        }
    }

    /*if (dynamicWedge != nullptr)
    {
        dynamicWedge->Render(renderData, measurePosition + offset);
    }*/

    /*if (bracketDirection != nullptr)
    {
        bracketDirection->Render(renderData, measurePosition + offset);
    }*/

    if (marker != nullptr)
    {
        marker->Render(renderData, measurePosition + offset);
    }
}

void Direction::RenderDebug(RenderData& renderData) const
{
    for (auto& word : words)
    {
        word.RenderDebug(renderData);
    }

    for (auto& rehearsal : rehearsals)
    {
        rehearsal.RenderDebug(renderData);
    }

    for (auto& dynamic : dynamics)
    {
        dynamic.RenderDebug(renderData);
    }

    if (metronomeMark != nullptr)
    {
        metronomeMark->RenderDebug(renderData);
    }

    if (marker)
    {
        marker->RenderDebug(renderData);
    }
}

Vec2<float> Direction::GetPositionRelativeToMeasure() const
{
    Vec2<float> position = { 0.0f, 0.0f };

    for (auto& wordsObject : words)
    {
        position = wordsObject.position;
    }

    for (auto& rehearsal : rehearsals)
    {
        position = rehearsal.position;
    }

    for (auto& dynamic : dynamics)
    {
        position = dynamic.position;
    }

    if (metronomeMark != nullptr)
    {
        position = metronomeMark->position;
    }

    if (dynamicWedge != nullptr)
    {
        position = dynamicWedge->position;
    }

    if (bracketDirection != nullptr)
    {
        position = bracketDirection->position;
    }

    if (marker != nullptr)
    {
        position = marker->position;
    }

    return position;
}

Vec2<float> Direction::GetDimensions() const
{
    Vec2<float> dimensions = { 0.0f, 0.0f };

    for (auto& wordsObject : words)
    {
        dimensions = wordsObject.GetDimensions();
    }

    for (auto& rehearsal : rehearsals)
    {
        dimensions = rehearsal.GetDimensions();
    }

    for (auto& dynamic : dynamics)
    {
        dimensions = dynamic.GetDimensions();
    }

    /*if (metronomeMark != nullptr)
    {
        position = metronomeMark->position;
    }

    if (dynamicWedge != nullptr)
    {
        position = dynamicWedge->position;
    }

    if (bracketDirection != nullptr)
    {
        position = bracketDirection->position;
    }

    if (marker != nullptr)
    {
        position = marker->position;
    }*/

    return dimensions;
}

BoundingBox Direction::GetBoundingBoxRelativeToMeasure() const
{
    Vec2<float> pos = GetPositionRelativeToMeasure();
    Vec2<float> size = GetDimensions();

    BoundingBox bb;

    bb.position = pos - (size / 2.0f);
    bb.size = size;

    return bb;
}

void Direction::UpdateBoundingBox(const Vec2<float>& measurePosition)
{
    for (auto& wordsObject : words)
    {
        wordsObject.UpdateBoundingBox(measurePosition);
    }

    for (auto& rehearsal : rehearsals)
    {
        rehearsal.UpdateBoundingBox(measurePosition);
    }

    for (auto& dynamic : dynamics)
    {
        dynamic.UpdateBoundingBox(measurePosition);
    }

    if (metronomeMark != nullptr)
    {
        metronomeMark->UpdateBoundingBox(measurePosition);
    }

    if (marker != nullptr)
    {
        marker->UpdateBoundingBox(measurePosition);
    }
}