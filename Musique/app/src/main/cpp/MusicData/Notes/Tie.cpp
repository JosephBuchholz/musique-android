#include "Tie.h"

#include "../../Utils/Math.h"

void NoteTie::Render(RenderData& renderData, Vec2<float> startNotePosition, Vec2<float> endNotePosition, bool isFirstNote) const
{
    if (isFirstNote && !isBroken) // the tie is not broken
    {
        Vec2<float> startCurvePosition = startNotePosition + positionStart;
        Vec2<float> endCurvePosition = endNotePosition + positionEnd;

        RenderCurve(renderData, startCurvePosition, endCurvePosition);
    }
    else if (isFirstNote && isBroken) // the first part of the broken tie
    {
        Vec2<float> startCurvePosition = startNotePosition + positionStart;
        Vec2<float> endCurvePosition = startNotePosition + positionStart;
        endCurvePosition.x += 10.0f;

        RenderCurve(renderData, startCurvePosition, endCurvePosition);
    }
    else if (!isFirstNote && isBroken) // the second part of the broken tie
    {
        Vec2<float> startCurvePosition = startNotePosition + positionEnd;
        startCurvePosition.x -= 10.0f;
        Vec2<float> endCurvePosition = startNotePosition + positionEnd;

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
    curve.point1.x = startPosition.x;
    curve.point1.y = startPosition.y;

    // curve points
    curve.point2.x = startPosition.x + curvePointDist;
    curve.point2.y = startPosition.y - curvePointDist * direction;

    curve.point3.x = endPosition.x - curvePointDist;
    curve.point3.y = endPosition.y - curvePointDist * direction;

    // end
    curve.point4.x = endPosition.x;
    curve.point4.y = endPosition.y;

    curve.paint = paint;
    curve.paint.varyThickness = true;

    renderData.AddCubicCurve(curve);
}

void NoteTie::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd, AboveBelowType defaultPlacement, bool isNoteChord)
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

    placement = defaultPlacement;

    if (placement == AboveBelowType::Above)
    {
        offsetY = -offsetY;
        orientation = CurveOrientation::Over;
    }
    else if (placement == AboveBelowType::Below)
    {
        offsetY = offsetY;
        orientation = CurveOrientation::Under;
    }

    positionStart.y += offsetY;
    positionEnd.y += offsetY;
}
