#include "Slur.h"

#include "../../Exceptions/Exceptions.h"
#include "../../Utils/Math.h"

void Slur::Render(RenderData& renderData, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, Vec2<float> offset) const
{
    Paint paint = renderData.paints.tiePaint;
    VisibleElement::ModifyPaint(paint);
    LineElement::ModifyPaint(paint);

    CubicCurve curve = CubicCurve();

    if (notes.empty())
        throw IsEmptyException();

    std::shared_ptr<Note> startNote = notes[0];
    std::shared_ptr<Note> endNote = notes[notes.size() - 1];

    Vec2<float> startCurvePosition = measureStartPosition + startNote->GetCenterPosition(renderData.displayConstants) + startPosition + offset;
    Vec2<float> endCurvePosition = measureEndPosition + endNote->GetCenterPosition(renderData.displayConstants) + endPosition + offset;

    float direction = 1.0f;
    if (orientation == CurveOrientation::Under)
        direction = -1.0f;

    float curvePointDist = FindDistance(startCurvePosition, endCurvePosition) / 7.0f;

    // start
    curve.point1.x = startCurvePosition.x;
    curve.point1.y = startCurvePosition.y;

    // curve points
    curve.point2.x = startCurvePosition.x + curvePointDist;
    curve.point2.y = startCurvePosition.y - curvePointDist * direction;

    curve.point3.x = endCurvePosition.x - curvePointDist;
    curve.point3.y = endCurvePosition.y - curvePointDist * direction;

    // end
    curve.point4.x = endCurvePosition.x;
    curve.point4.y = endCurvePosition.y;

    curve.paint = paint;
    curve.paint.varyThickness = true;

    renderData.AddCubicCurve(curve);
}

void Slur::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{
    float offsetY = 0.0f;

    if (placement == AboveBelowType::Above)
        offsetY = -10.0f;
    else if (placement == AboveBelowType::Below)
        offsetY = 10.0f;

    startPosition = { 0.0f, 0.0f + offsetY };
    endPosition = { 0.0f, 0.0f + offsetY };
}