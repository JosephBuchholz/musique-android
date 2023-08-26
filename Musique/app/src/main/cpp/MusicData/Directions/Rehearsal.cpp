#include "Rehearsal.h"

void Rehearsal::Render(RenderData& renderData, Vec2<float> measurePosition) const
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    renderData.AddText(Text(text.string, position.x + measurePosition.x, position.y + measurePosition.y, Paint(color.color, paint)));
    //renderData.AddDebugDot(positionX + measurePosition.x + offset.x, positionY + measurePosition.y + offset.y);
}