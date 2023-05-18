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

    if (dynamicWedge != nullptr)
    {
        dynamicWedge->Render(renderData, measurePosition + offset);
    }

    if (bracketDirection != nullptr)
    {
        bracketDirection->Render(renderData, measurePosition + offset);
    }

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

    if (dynamicWedge != nullptr)
    {
        dynamicWedge->RenderDebug(renderData);
    }

    if (bracketDirection != nullptr)
    {
        bracketDirection->RenderDebug(renderData);
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

    if (dynamicWedge != nullptr)
    {
        dynamicWedge->UpdateBoundingBox(measurePosition);
    }

    if (bracketDirection != nullptr)
    {
        bracketDirection->UpdateBoundingBox(measurePosition);
    }

    if (marker != nullptr)
    {
        marker->UpdateBoundingBox(measurePosition);
    }
}