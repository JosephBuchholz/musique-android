#include "Tie.h"

#include "../../Utils/Math.h"

void NoteTie::Render(RenderData& renderData, Vec2<float> startNotePosition, Vec2<float> endNotePosition, bool isFirstNote, Vec2<float> offset) const
{
    if (isFirstNote && !isBroken) // the tie is not broken
    {
        Vec2<float> startCurvePosition = startNotePosition + positionStart + offset;
        Vec2<float> endCurvePosition = endNotePosition + positionEnd + offset;

        RenderCurve(renderData, startCurvePosition, endCurvePosition);
    }
    else if (isFirstNote && isBroken) // the first part of the broken tie
    {
        Vec2<float> startCurvePosition = startNotePosition + positionStart + offset;
        Vec2<float> endCurvePosition = startNotePosition + positionStart + offset;
        endCurvePosition.x += 15.0f;

        RenderCurve(renderData, startCurvePosition, endCurvePosition);
    }
    else if (!isFirstNote && isBroken) // the second part of the broken tie
    {
        Vec2<float> startCurvePosition = startNotePosition + positionEnd + offset;
        startCurvePosition.x -= 15.0f;
        Vec2<float> endCurvePosition = startNotePosition + positionEnd + offset;

        RenderCurve(renderData, startCurvePosition, endCurvePosition);
    }
}

void NoteTie::RenderCurve(RenderData& renderData, Vec2<float> startPosition, Vec2<float> endPosition) const
{
    Paint paint = renderData.paints.tiePaint;
    VisibleElement::ModifyPaint(paint);
    LineElement::ModifyPaint(paint);

    CubicCurve curve = CubicCurve();

    float direction = 1.0f;
    if (orientation == CurveOrientation::Under)
        direction = -1.0f;

    float curvePointDist = FindDistance(startPosition, endPosition) / 7.0f;

    // start
    curve.x1 = startPosition.x;
    curve.y1 = startPosition.y;

    // curve points
    curve.x2 = startPosition.x + curvePointDist;
    curve.y2 = startPosition.y - curvePointDist * direction;

    curve.x3 = endPosition.x - curvePointDist;
    curve.y3 = endPosition.y - curvePointDist * direction;

    // end
    curve.x4 = endPosition.x;
    curve.y4 = endPosition.y;

    curve.paint = paint;

    renderData.AddCubicCurve(curve);
}

void NoteTie::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd, bool isNoteChord)
{
    positionStart = defPositionStart;
    positionEnd = defPositionEnd;

    float offsetY = 10.0f;

    if (isNoteChord)
    {
        positionStart.x += 10.0f;
        positionEnd.x -= 10.0f;

        offsetY = 0.0f;
    }

    if (placement == AboveBelowType::Above)
        offsetY = -offsetY;
    //else if (placement == AboveBelowType::Below)
    //    offsetY = offsetY;

    positionStart.y += offsetY;
    positionEnd.y += offsetY;
}
