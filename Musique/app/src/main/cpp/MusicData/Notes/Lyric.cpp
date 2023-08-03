#include "Lyric.h"

void Lyric::Render(RenderData& renderData, float notePositionX, float measurePositionY, float offsetX, float offsetY) const
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    renderData.AddText(Text(text[0].text, { position.x + notePositionX + offsetX, position.y + measurePositionY + offsetY }, Paint(color.color, paint)));
}

void Lyric::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    Paint paint = Paint();
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize * 2.0f;
    bb.size.x = (paint.textSize * (float)text[0].text.size()) + (paint.textSize * 2.0f);
    bb.size.y = paint.textSize * 2.0f;

    boundingBox.position.x = bb.position.x + position.x + parentPosition.x;
    boundingBox.position.y = bb.position.y + position.y + parentPosition.y;
    boundingBox.position.x -= bb.size.x / 2.0f;
    boundingBox.position.y += bb.size.y / 2.0f;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void Lyric::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{
    position.x = defX;
    position.y = -defY;

    position.x += relX;
    position.y -= relY;
}