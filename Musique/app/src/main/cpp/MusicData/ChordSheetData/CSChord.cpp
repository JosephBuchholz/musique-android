#include "CSChord.h"

void CSChord::Render(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition) const
{
    //LOGI_TAG("CSChord.cpp", "Rendering chord!!!!: mpos: %s | pos: %s", measurePosition.GetPrintableString().c_str(), position.GetPrintableString().c_str());
    chordSymbol.Render(renderData, settings, measurePosition + position);
}

void CSChord::Init(Vec2<float> pos)
{
    position = pos;
}

Vec2<float> CSChord::GetDimensions() const
{
    return chordSymbol.GetBoundingBoxRelativeToParent().size;
}
