#include "CSMeasure.h"

void CSMeasure::Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const
{

    LOGV_TAG("CSMeasure", "Rendering Measure!");
    for (const auto& chord : chords)
    {
        chord.Render(renderData, settings, parentPosition + position);
    }
}