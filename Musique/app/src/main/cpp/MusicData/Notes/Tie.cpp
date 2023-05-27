#include "Tie.h"

#include "../../Utils/Math.h"

void NoteTie::Render(RenderData& renderData, Vec2<float> startNotePosition, Vec2<float> endNotePosition, Vec2<float> offset) const
{
    Paint paint = renderData.paints.tiePaint;
    VisibleElement::ModifyPaint(paint);
    LineElement::ModifyPaint(paint);

    CubicCurve curve = CubicCurve();

    Vec2<float> startCurvePosition = startNotePosition + positionStart + offset;
    Vec2<float> endCurvePosition = endNotePosition + positionEnd + offset;

    float direction = 1.0f;
    if (orientation == CurveOrientation::Under)
        direction = -1.0f;

    float curvePointDist = FindDistance(startCurvePosition, endCurvePosition) / 7.0f;

    // start
    curve.x1 = startCurvePosition.x;
    curve.y1 = startCurvePosition.y;

    // curve points
    curve.x2 = startCurvePosition.x + curvePointDist;
    curve.y2 = startCurvePosition.y - curvePointDist * direction;

    curve.x3 = endCurvePosition.x - curvePointDist;
    curve.y3 = endCurvePosition.y - curvePointDist * direction;

    // end
    curve.x4 = endCurvePosition.x;
    curve.y4 = endCurvePosition.y;

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
