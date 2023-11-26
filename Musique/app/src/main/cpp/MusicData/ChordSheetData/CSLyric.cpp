#include "CSLyric.h"

void CSLyric::Init(Vec2<float> pos)
{
    lyricText.fontSize.size = 8.0f;
    lyricText.align = TextAlign::Left;

    position = pos;
}

void CSLyric::Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const
{
    lyricText.Render(renderData, parentPosition + position, Paint());
/*
#if DEBUG
    if (isPickupToNextMeasure)
        lyricText.GetBoundingBox(Paint()).Render(renderData, Color(0xFFFF0000), parentPosition + position);
    else
        lyricText.GetBoundingBox(Paint()).Render(renderData, Color(0xFF00FF00), parentPosition + position);
#endif*/
}


