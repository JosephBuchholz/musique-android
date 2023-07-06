#include "DurationDirection.h"

void DurationDirectionSegment::Move(Vec2<float> positionOffset, Vec2<float> sizeOffset, float rotationOffset)
{
    positionStart += positionOffset;
    positionEnd += positionOffset;
    rotation += rotationOffset;
}