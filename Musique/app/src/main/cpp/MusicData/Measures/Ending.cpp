#include "Ending.h"

void Ending::Render(RenderData& renderData, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, Vec2<float> offset) const
{
    // render lines

    LOGE("Rendering ending: measureStartPos: %s, measureEndPos: %s, startMeasureIndex: %d, endMeasureIndex: %d", measureStartPosition.GetPrintableString().c_str(), measureEndPosition.GetPrintableString().c_str(), startMeasureIndex, endMeasureIndex);

    Paint linePaint;
    VisibleElement::ModifyPaint(linePaint);
    LineElement::ModifyPaint(linePaint);

    //Vec2<float> startPosition = { position.x + measurePosition.x + offset.x, position.y + measurePosition.y + offset.y };
    //Vec2<float> endPosition = { startPosition.x + measureWidth, startPosition.y };

    /*float endingWidth = 0.0f;
    for (int i = startMeasureIndex; i < endMeasureIndex; i++)
    {
        endingWidth += measureWidths[i];
    }*/

    Vec2<float> startPosition = { position.x + measureStartPosition.x + offset.x, position.y + measureStartPosition.y + offset.y };
    Vec2<float> endPosition = { position.x + measureEndPosition.x + offset.x, position.y + measureEndPosition.y + offset.y };

    // top line
    renderData.AddLine(startPosition.x, startPosition.y, endPosition.x, endPosition.y, linePaint);

    // front jog
    renderData.AddLine(startPosition.x, startPosition.y, startPosition.x, startPosition.y + jogLength, linePaint);

    // final jog
    if (hasFinalDownwardJog)
        renderData.AddLine(endPosition.x, endPosition.y, endPosition.x, endPosition.y + jogLength, linePaint);

    // render text

    Paint textPaint;
    TextualElement::ModifyPaint(textPaint);
    textPaint.align = Paint::Align::Left;

    renderData.AddText(Text(endingNumbersText, textPosition.x + startPosition.x, textPosition.y + startPosition.y, textPaint));
}

void Ending::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{
    if (defaultPosition.y != INVALID_FLOAT_VALUE)
    {
        position.y = -defaultPosition.y;
    }
    else
    {
        position.y = -30.0f;
    }

    textPosition = { 5.0f, 10.0f };
}