#include "Words.h"

#include "../../RenderMeasurement.h"

void Words::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    Paint paint = Paint();
    //paint.textSize = 16.0f;
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size; // ERROROROROROR

    //LOGV("paint.textSize: %f!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1", paint.textSize);

#define TOGGLE 0

#if TOGGLE
    BoundingBox bb = RenderMeasurement::GetTextBoundingBox(Text(text.string, positionX, positionY, paint));
#else
    BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize * 2.0f;
    bb.size.x = paint.textSize * (float)text.string.size();
    bb.size.y = paint.textSize * 2.0f;
#endif

    boundingBox.position.x = bb.position.x + positionX + parentPosition.x;
    boundingBox.position.y = bb.position.y + positionY + parentPosition.y;
    boundingBox.position.x -= bb.size.x / 2.0f;
    boundingBox.position.y += bb.size.y / 2.0f;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif

    LOGV("Updated bounding box for WORDS: %s", boundingBox.GetPrintableString().c_str());
    LOGV("parentPosition: %s", parentPosition.GetPrintableString().c_str());
    LOGD("boundingBoxPosition: %s", bb.GetPrintableString().c_str());
}

void Words::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY)
{
    /*if (noDefX)
        positionX = defaultX + relX;
    else
        positionX = defX + relX;*/

    if (noDefY)
        positionY = defaultY + -relY;
    else
        positionY = -defY + -relY;

    positionX = defaultX;
    //positionY = defaultY;
}
