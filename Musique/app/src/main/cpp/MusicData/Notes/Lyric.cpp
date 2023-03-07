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

    renderData.AddText(Text(text[0].text, positionX + notePositionX + offsetX, positionY + measurePositionY + offsetY, Paint(color.color, paint)));
}

void Lyric::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{
    positionX = defX;
    positionY = -defY;

    positionX += relX;
    positionY -= relY;
}