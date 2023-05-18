#include "Rehearsal.h"

void Rehearsal::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    renderData.AddText(Text(text.string, positionX + measurePosition.x + offset.x, positionY + measurePosition.y + offset.y, Paint(color.color, paint)));
}