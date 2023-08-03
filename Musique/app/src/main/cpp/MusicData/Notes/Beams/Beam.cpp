#include "Beam.h"

void Beam::Render(RenderData& renderData, Vec2<float> measurePosition, float slope, float yIntercept, bool isAboveNote, Vec2<float> offset) const
{
    const float beamDistance = 8.0f * size;
    const float hookOffset = 10.0f * size;
    float beamYOffset = (beamDistance * float(beamLevel-1));

    Paint paint = renderData.paints.noteBeamPaint;
    VisibleElement::ModifyPaint(paint);
    LineElement::ModifyPaint(paint);
    paint.strokeWidth *= size;

    if (isAboveNote)
    {
        beamYOffset = -beamYOffset;
    }

    if (beamType == Beam::BeamType::Normal)
    {
        renderData.AddLine(Line({ beamStartPosition.x + measurePosition.x + offset.x, beamStartPosition.y + measurePosition.y + offset.y - beamYOffset }, { beamEndPosition.x + measurePosition.x + offset.x, beamEndPosition.y + measurePosition.y + offset.y - beamYOffset }, paint));
    }
    else if (beamType == Beam::BeamType::ForwardHook)
    {
        renderData.AddLine(Line({ beamStartPosition.x + measurePosition.x + offset.x, beamStartPosition.y + measurePosition.y + offset.y - beamYOffset }, { beamStartPosition.x + measurePosition.x + offset.x + hookOffset, GetPositionYOnBeam(beamStartPosition.x + hookOffset, slope, yIntercept) + measurePosition.y + offset.y - beamYOffset }, paint));
    }
    else if (beamType == Beam::BeamType::BackwardHook)
    {
        Vec2<float> start = { beamStartPosition.x - hookOffset, GetPositionYOnBeam(beamStartPosition.x - hookOffset, slope, yIntercept) - beamYOffset };
        Vec2<float> end = { beamStartPosition.x, beamStartPosition.y - beamYOffset };
        renderData.AddLine(Line({ start.x + measurePosition.x + offset.x, start.y + measurePosition.y + offset.y }, { end.x + measurePosition.x + offset.x, end.y + measurePosition.y + offset.y }, paint));
    }
}

void Beam::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition)
{
    boundingBox.position = beamStartPosition + parentPosition;
    boundingBox.size = beamEndPosition - beamStartPosition;
    boundingBox.MakeDimensionsPositive();
    boundingBox.AddPadding(displayConstants.beamLineWidth);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void Beam::CalculateAsPaged(const MusicDisplayConstants& displayConstants, float size)
{
    this->size = size;
}

float Beam::GetPositionYOnBeam(float positionX, float slope, float yIntercept) const
{
    return (slope * positionX) + yIntercept; // y = mx + b
}