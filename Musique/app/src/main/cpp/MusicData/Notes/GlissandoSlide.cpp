#include "GlissandoSlide.h"

#include "../../Utils/Math.h"

void GlissandoSlide::UpdateBoundingBox(const Vec2<float>& parentPositionCenter)
{
#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void GlissandoSlide::Render(RenderData& renderData, Vec2<float> startNotePosition, Vec2<float> endNotePosition, bool isFirstNote, Vec2<float> offset) const
{
    LOGE("gliss is broken: %d", isBroken);
    if (isFirstNote && !isBroken) // not broken
    {
        RenderLine(renderData, positionStart + startNotePosition + offset, positionEnd + endNotePosition + offset);
    }
    else if (isFirstNote && isBroken) // the first part of the broken gliss/slide
    {
        Vec2<float> startPos = startNotePosition + positionStart + offset;
        Vec2<float> endPos;
        endPos.x = startNotePosition.x + positionStart.x + offset.x;
        endPos.y = startNotePosition.y + positionEnd.y + offset.y;
        endPos.x += 15.0f;

        RenderLine(renderData, startPos, endPos);
    }
    else if (!isFirstNote && isBroken) // the second part of the broken gliss/slide
    {
        Vec2<float> startPos;
        startPos.x = startNotePosition.x + positionEnd.x + offset.x;
        startPos.y = startNotePosition.y + positionStart.y + offset.y;
        startPos.x -= 15.0f;
        Vec2<float> endPos = startNotePosition + positionEnd + offset;

        RenderLine(renderData, startPos, endPos);
    }

    return; // TODO: finnish display text
    if (displayText)
    {
        Paint textPaint;
        TextualElement::ModifyPaint(textPaint);

        textPaint.textSize = 8.0f;
        textPaint.isItalic = true;

        Vec2<float> endPosRelativeToStart = (positionEnd + endNotePosition) - (positionStart + startNotePosition);

        float angleDegrees = 0.0f;

        if (endPosRelativeToStart.x != 0.0f && endPosRelativeToStart.y != 0.0f)
        {
            float angleRadians = std::atan(endPosRelativeToStart.y / endPosRelativeToStart.x);
            angleDegrees = RadiansToDegrees(angleRadians);

            if (endPosRelativeToStart.x < 0.0f)
                angleDegrees += 180.0f;
        }
        else
        {
            if (endPosRelativeToStart.x > 0.0f)
                angleDegrees = 0.0f;
            else if (endPosRelativeToStart.y > 0.0f)
                angleDegrees = 90.0f;
            else if (endPosRelativeToStart.x < 0.0f)
                angleDegrees = 180.0f;
            else // endPosRelativeToStart.y < 0.0f || (endPosRelativeToStart.x == 0 && endPosRelativeToStart.y == 0)
                angleDegrees = 270.0f;
        }

        textPaint.rotateDegrees = -angleDegrees;

        Vec2<float> textPosition = { 0.0f, 0.0f };

        textPosition.x = (positionStart.x + startNotePosition.x) + (((positionEnd.x + endNotePosition.x) - (positionStart.x + startNotePosition.x)) / 2.0f);
        textPosition.y = (positionStart.y + startNotePosition.y) + (((positionEnd.y + endNotePosition.y) - (positionStart.y + startNotePosition.y)) / 2.0f);

        renderData.AddText(Text(text, textPosition + offset, textPaint));
    }
}

void GlissandoSlide::RenderDebug(RenderData& renderData) const
{
#if DEBUG_BOUNDING_BOXES
    debugBoundingBox.Render(renderData, 0xFF00FF00);
#endif

    boundingBox.Render(renderData, 0xFF00FFFF);
}

void GlissandoSlide::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd, bool isTab)
{
    positionStart = defPositionStart;
    positionEnd = defPositionEnd;

    if (notes.first->pitch <= notes.second->pitch) // up
    {
        isSlideUp = true;
    }
    else // down
    {
        isSlideUp = false;
    }

    float dist = 2.0f;

    if (isTab)
        dist = 4.0f;

    if (isSlideUp)
    {
        positionStart.y += dist;
        positionEnd.y -= dist;
    }
    else // !isSlideUp
    {
        positionStart.y -= dist;
        positionEnd.y += dist;
    }

    if (lineType == LineType::Wavy)
    {
        if (complexLine == nullptr)
        {
            complexLine = std::make_shared<ComplexLine>(ComplexLine::ComplexLineType::WavyGliss);
        }

        complexLine->positionStart = positionStart;
        complexLine->positionEnd = positionEnd;
    }
}

void GlissandoSlide::RenderLine(RenderData& renderData, Vec2<float> startPosition, Vec2<float> endPosition) const
{
    Paint linePaint;
    VisibleElement::ModifyPaint(linePaint);
    LineElement::ModifyPaint(linePaint);
    linePaint.strokeWidth = 1.6f;

    if (lineType == LineType::Wavy)
    {
        complexLine->positionStart = startPosition;
        complexLine->positionEnd = endPosition;

        complexLine->Render(renderData, { 0.0f, 0.0f });
    }
    else
    {
        renderData.AddLine(Line(startPosition, endPosition, linePaint));
    }
}