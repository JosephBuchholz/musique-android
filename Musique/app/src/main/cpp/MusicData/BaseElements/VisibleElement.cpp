#include "VisibleElement.h"

void VisibleElement::RenderDebug(RenderData& renderData) const
{
#if DEBUG_BOUNDING_BOXES
    debugBoundingBox.Render(renderData, 0xFF00FF00);
#endif

    boundingBox.Render(renderData, 0xFF00FFFF);
}