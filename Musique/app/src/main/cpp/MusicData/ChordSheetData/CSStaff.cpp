#include "CSStaff.h"

void CSStaff::Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const
{
    for (const auto& measure : measures)
    {
        measure->Render(renderData, settings, parentPosition + position);
    }
}