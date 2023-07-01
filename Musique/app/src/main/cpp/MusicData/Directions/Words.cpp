#include "Words.h"

#include "../../RenderMeasurement.h"

void Words::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    renderData.AddText(Text(text.string, position.x + measurePosition.x + offset.y, position.y + measurePosition.y + offset.y, Paint(color.color, paint)));
}

Vec2<float> Words::GetDimensions() const
{
    Vec2<float> dimensions;
    dimensions.x = fontSize.size * (float)text.string.size();
    dimensions.y = fontSize.size * 2.0f;
    return dimensions;
}

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
    //bb.position.y = 0.0f;
    bb.size.x = paint.textSize * (float)text.string.size();
    bb.size.y = paint.textSize * 2.0f;
#endif

    boundingBox.position.x = bb.position.x + position.x + parentPosition.x;
    boundingBox.position.y = bb.position.y + position.y + parentPosition.y;
    boundingBox.position.x -= bb.size.x / 2.0f;
    boundingBox.position.y += bb.size.y / 2.0f;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::None);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif

    /*LOGV("Updated bounding box for WORDS: %s", boundingBox.GetPrintableString().c_str());
    LOGV("parentPosition: %s", parentPosition.GetPrintableString().c_str());
    LOGD("boundingBoxPosition: %s", bb.GetPrintableString().c_str());*/
}

void Words::Move(Vec2<float> positionOffset, Vec2<float> sizeOffset, float rotationOffset)
{
    position.x += positionOffset.x;
    position.y += positionOffset.y;
}

void Words::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY)
{
    /*if (noDefX)
        positionX = defaultX + relX;
    else
        positionX = defX + relX;*/

    //LOGE("calculate position: x: %f, y: %f", defaultX, defaultY);

    if (noDefY)
        position.y = defaultY + -relY;
    else
        position.y = -defY + -relY;

    position.x = defaultX;
    //positionY = defaultY;
}
