#include "GlissandoSlide.h"

#include "../../Utils/Math.h"

void GlissandoSlide::UpdateBoundingBox(const Vec2<float>& parentPositionCenter)
{
#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void GlissandoSlide::Render(RenderData& renderData, Vec2<float> startNotePosition, Vec2<float> endNotePosition, Vec2<float> offset) const
{
    Paint linePaint;
    VisibleElement::ModifyPaint(linePaint);
    LineElement::ModifyPaint(linePaint);
    linePaint.strokeWidth = 1.6f;

    if (lineType == LineType::Wavy)
    {
        complexLine->positionStart = positionStart + startNotePosition + offset;
        complexLine->positionEnd = positionEnd + endNotePosition + offset;

        complexLine->Render(renderData, { 0.0f, 0.0f });
    }
    else
    {
        renderData.AddLine(std::make_shared<Line>(positionStart.x + startNotePosition.x + offset.x, positionStart.y + startNotePosition.y + offset.y, positionEnd.x + endNotePosition.x + offset.x, positionEnd.y + endNotePosition.y + offset.y, linePaint));
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

        renderData.AddText(Text(text, textPosition.x + offset.x, textPosition.y + offset.y, textPaint));
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