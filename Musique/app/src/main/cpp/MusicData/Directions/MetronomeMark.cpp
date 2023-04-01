#include "MetronomeMark.h"

void MetronomeMark::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    Paint paint = Paint();

    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize;
    bb.size.x = paint.textSize * ((float)tempo.size() + 5);
    bb.size.y = paint.textSize * 3.0f;

    boundingBox.position.x = bb.position.x + positionX + parentPosition.x;
    boundingBox.position.y = bb.position.y + positionY + parentPosition.y;
    boundingBox.position.y -= bb.size.y / 2.0f;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void MetronomeMark::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY)
{
    if (noDefX)
        positionX = defaultX + relX;
    else
        positionX = defX + relX;

    if (noDefY)
        positionY = defaultY + -relY;
    else
        positionY = -defY + -relY;
}