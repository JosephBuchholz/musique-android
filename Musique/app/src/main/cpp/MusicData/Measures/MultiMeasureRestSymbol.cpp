#include "MultiMeasureRestSymbol.h"

#include "TimeSignature.h"

const float hBarSideMargin = 8.0f;
const float hBarMidThickness = 8.0f;

const float hBarTotalHeight = 20.0f;
const float hBarOutsideLineThickness = 1.75f;

void MultiMeasureRestSymbol::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    //int spaces = lineCount - 1;
    //float hBarYPosition = (lineSpacing * ((float)spaces/2.0f)) + measurePosition.y; // the middle of staff
    //float hBarYPosition = positionY + measurePosition.y;

    Paint midLinePaint = Paint();
    VisibleElement::ModifyPaint(midLinePaint);
    midLinePaint.strokeWidth = hBarMidThickness;
    midLinePaint.strokeCap = Paint::Cap::Butt;

    float hBarPositionY = positionY + measurePosition.y + offset.y;
    float hBarStartPositionX = startPositionX + measurePosition.x + offset.x;
    float hBarEndPositionX = endPositionX + measurePosition.x + offset.x;

    // middle line
    renderData.AddLine(Line({ hBarStartPositionX, hBarPositionY }, { hBarEndPositionX, hBarPositionY }, midLinePaint ));

    Paint sideLinePaint = Paint();
    VisibleElement::ModifyPaint(sideLinePaint);
    sideLinePaint.strokeWidth = hBarOutsideLineThickness;
    sideLinePaint.strokeCap = Paint::Cap::Butt;

    // side lines
    renderData.AddLine(Line({ hBarStartPositionX, hBarPositionY - (hBarTotalHeight/2.0f) }, { hBarStartPositionX, hBarPositionY + (hBarTotalHeight/2.0f) }, sideLinePaint));
    renderData.AddLine(Line({ hBarEndPositionX, hBarPositionY - (hBarTotalHeight/2.0f) }, { hBarEndPositionX, hBarPositionY + (hBarTotalHeight/2.0f) }, sideLinePaint));

    // render digits above

    Paint digitPaint;
    VisibleElement::ModifyPaint(digitPaint);

    std::string numberString = ToString(numberOfMeasures);
    float timeDigitWidth = 16.0f;
    float totalWidth = timeDigitWidth * (float)numberString.length();
    float textStartPositionX = textPosition.x - (totalWidth / 2.0f) + measurePosition.x + offset.x;
    //float startPositionX = ((measureWidth / 2.0f) - (totalWidth / 2.0f)) + measurePosition.x;

    float textPositionY = textPosition.y + measurePosition.y + offset.y;

    for (const char& c : numberString)
    {
        SMuFLID glyphID = TimeSignature::GetTimeSignatureSMuFLID(ToInt(c));

        renderData.AddGlyph(SMuFLGlyph(glyphID, { textStartPositionX, textPositionY }, digitPaint));

        textStartPositionX += timeDigitWidth;
    }

    /*renderData.AddDebugDot(hBarStartPositionX, hBarPositionY);
    renderData.AddDebugDot(hBarEndPositionX, hBarPositionY);
    renderData.AddDebugDot(textStartPositionX, textPositionY);
    renderData.AddDebugDot(textPosition.x + measurePosition.x, textPosition.y + measurePosition.y);*/
}

void MultiMeasureRestSymbol::CalculateAsPaged(const MusicDisplayConstants& displayConstants, float lineSpacing, int staffLines, float measureWidth, float measureBeginWidth)
{
    int spaces = staffLines - 1;
    positionY = (lineSpacing * ((float)spaces/2.0f));

    startPositionX = hBarSideMargin + measureBeginWidth - 5.0f;
    float hBarWidth = measureWidth - startPositionX - hBarSideMargin;
    endPositionX = startPositionX + hBarWidth;

    textPosition.x = startPositionX + (hBarWidth / 2.0f);
    textPosition.y = -20.0f;
}