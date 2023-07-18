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