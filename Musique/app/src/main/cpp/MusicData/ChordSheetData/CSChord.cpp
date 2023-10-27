#include "CSChord.h"

void CSChord::Render(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition) const
{
    chordSymbol.Render(renderData, settings, measurePosition + position);
}
