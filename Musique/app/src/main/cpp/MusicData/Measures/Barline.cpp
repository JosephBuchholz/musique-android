#include "Barline.h"

void Barline::Render(RenderData& renderData, Vec2<float> measurePosition, float height, float lineSpacing, int lineCount, bool isTopStaff, bool isPartOfEnding, Vec2<float> offset) const
{
    float positionY = 0.0f;

    float barlinePositionYBottom = positionY + height + measurePosition.y + offset.y;

    float doubleBarlineOffset = 5.0f;
    float doubleBarlineOffsetDirection = 1.0f;

    // dots for repeat barlines

    if (isRepeatBarLine)
    {
        Paint dotsPaint;
        VisibleElement::ModifyPaint(dotsPaint);

        // TODO: fix repeat dot horizontal positioning
        float dotsPositionX = 0.0f;

        float barlineWidth = doubleBarlineOffset + 5.0f;
        if (facing == Barline::Direction::Forward)
            dotsPositionX = positionX + (barlineWidth + 5.0f);
        else if (facing == Barline::Direction::Backward)
            dotsPositionX = positionX - (barlineWidth + 5.0f) - 7.0f;

        if (location == Barline::Location::Right)
        {
            doubleBarlineOffsetDirection = -1.0f;
        }
        else if (location == Barline::Location::Left)
            doubleBarlineOffsetDirection = 0.0f;

        int spaces = lineCount - 1; // the number of spaces on the staff

        // the space that has the top most dot
        int topDotSpace = int(spaces / 2); // floor(spaces / 2)

        // the space that has the bottom most dot
        int bottomDotSpace = int((float)spaces / 2.0f + 1.5f); // floor

        float dotsPositionY = positionY + ((float)topDotSpace * lineSpacing) - (0.5f * lineSpacing); // barlineY + the spaces (scaled to lineSpcaing) - half a space to center the dot
        renderData.AddGlyph(SMuFLGlyph(SMuFLID::repeatDot, { dotsPositionX + measurePosition.x + offset.x, dotsPositionY + measurePosition.y + offset.y }, dotsPaint));

        // same thing but with bottomDotSpace
        dotsPositionY = positionY + ((float)bottomDotSpace * lineSpacing) - (0.5f * lineSpacing);
        renderData.AddGlyph(SMuFLGlyph(SMuFLID::repeatDot, { dotsPositionX + measurePosition.x + offset.x, dotsPositionY + measurePosition.y + offset.y }, dotsPaint));
    }

    float direction = 0.0f;

    // actual bar line

    if (location == Barline::Location::Right)
    {
        doubleBarlineOffsetDirection = -1.0f;
        direction = -1.0f;
    }
    else if (location == Barline::Location::Left)
    {
        doubleBarlineOffsetDirection = 0.0f;
        direction = 1.0f;
    }

    float barlineWidth = 5.0f;

    Paint paint = renderData.paints.barlinePaint;
    VisibleElement::ModifyPaint(paint);
    LineElement::ModifyPaint(paint);

    Paint heavyBarlinePaint = renderData.paints.heavyBarlinePaint;
    VisibleElement::ModifyPaint(heavyBarlinePaint);
    LineElement::ModifyPaint(heavyBarlinePaint);

    float barlinePositionX = positionX + measurePosition.x + offset.x;
    float barlinePositionY = positionY + measurePosition.y + offset.y;
    switch (barlineStyle)
    {
        case Barline::BarlineStyle::Regular:
        {
            renderData.AddLine(Line({ positionX + measurePosition.x + offset.x, positionY + measurePosition.y + offset.y }, { positionX + measurePosition.x + offset.x, barlinePositionYBottom + measurePosition.y + offset.y }, paint));
            break;
        }
        case Barline::BarlineStyle::LightLight:
        {
            barlineWidth = doubleBarlineOffset + (paint.strokeWidth * 2.0f);
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            barlinePositionX += paint.strokeWidth / 2.0f;
            renderData.AddLine(Line({ barlinePositionX, barlinePositionY }, { barlinePositionX, barlinePositionYBottom }, paint));
            barlinePositionX += doubleBarlineOffset + paint.strokeWidth;
            renderData.AddLine(Line({ barlinePositionX, barlinePositionY }, { barlinePositionX, barlinePositionYBottom }, paint));
            break;
        }
        case Barline::BarlineStyle::HeavyLight:
        {
            barlineWidth = doubleBarlineOffset + heavyBarlinePaint.strokeWidth;
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;

            //barlinePositionX += heavyBarlinePaint.strokeWidth / 2.0f;
            renderData.AddLine(Line({ barlinePositionX, barlinePositionY }, { barlinePositionX, barlinePositionYBottom }, heavyBarlinePaint));

            barlinePositionX += doubleBarlineOffset + (heavyBarlinePaint.strokeWidth / 2.0f);
            renderData.AddLine(Line({ barlinePositionX, barlinePositionY }, { barlinePositionX, barlinePositionYBottom }, paint));
            break;
        }
        case Barline::BarlineStyle::LightHeavy:
        {
            barlineWidth = doubleBarlineOffset + heavyBarlinePaint.strokeWidth;
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            renderData.AddLine(Line({ barlinePositionX, barlinePositionY }, { barlinePositionX, barlinePositionYBottom }, paint));
            barlinePositionX += doubleBarlineOffset + (heavyBarlinePaint.strokeWidth / 2.0f);
            renderData.AddLine(Line({ barlinePositionX, barlinePositionY }, { barlinePositionX, barlinePositionYBottom }, heavyBarlinePaint));
            break;
        }
        case Barline::BarlineStyle::HeavyHeavy:
        {
            barlineWidth = doubleBarlineOffset + (heavyBarlinePaint.strokeWidth * 2.0f);
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            barlinePositionX += heavyBarlinePaint.strokeWidth / 2.0f;
            renderData.AddLine(Line({ barlinePositionX, barlinePositionY }, { barlinePositionX, barlinePositionYBottom }, heavyBarlinePaint));
            barlinePositionX += doubleBarlineOffset + (heavyBarlinePaint.strokeWidth);
            renderData.AddLine(Line({ barlinePositionX, barlinePositionY }, { barlinePositionX, barlinePositionYBottom }, heavyBarlinePaint));
            break;
        }
        case Barline::BarlineStyle::NoBarline:
        default: break; // do nothing
    }

    if (repeatNumberDirection && isTopStaff && !isPartOfEnding)
    {
        repeatNumberDirection->Render(renderData, measurePosition);
    }
}

void Barline::CalculateAsPaged(const MusicDisplayConstants& displayConstants, float measureWidth, float repeatBarlineOffset)
{
    positionX = 0.0f;

    float barlineLeftPositionX = 0.0f;
    float barlineRightPositionX = barlineLeftPositionX + measureWidth;

    if (location == Barline::Location::Right)
        positionX = barlineRightPositionX;
    else if (location == Barline::Location::Left)
    {
        positionX = barlineLeftPositionX;

        if (isRepeatBarLine)
            positionX += repeatBarlineOffset;
    }
    else if (location == Barline::Location::Middle)
        LOGW("Barline: That barline location is not supported");

    if (isRepeatBarLine && facing == Direction::Backward && repeatCount > 2)
    {
        repeatNumberDirection = std::make_shared<Words>();
        repeatNumberDirection->text = ToString(repeatCount) + "x";
        repeatNumberDirection->position.x = positionX;
        repeatNumberDirection->position.y = -20.0f;
        repeatNumberDirection->fontSize.size = 8.0f;
    }
}