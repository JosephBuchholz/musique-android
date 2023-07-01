#include "BeamGroup.h"

void BeamGroup::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    float slope = (beamStartPosition.y - beamEndPosition.y) / (beamStartPosition.x - beamEndPosition.x); // the slope of the beam
    float yIntercept = beamStartPosition.y - (slope * beamStartPosition.x); // solve for b (b = y - mx)

    for (const Beam& beam : beams)
    {
        beam.Render(renderData, measurePosition, slope, yIntercept, isAboveNote, offset);
    }

    //renderData.AddLine(std::make_shared<Line>(beamStartPosition.x + measurePosition.x + offset.x, beamStartPosition.y + measurePosition.y + offset.y, beamEndPosition.x + measurePosition.x + offset.x, beamEndPosition.y + measurePosition.y + offset.y, Paint(0xFFFF0000)));
}

void BeamGroup::RenderDebug(RenderData& renderData) const
{
    for (const Beam& beam : beams)
    {
        beam.RenderDebug(renderData);
    }

    boundingBox.Render(renderData, 0xFFFF0000);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox.Render(renderData);
#endif
}

BoundingBox BeamGroup::GetBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const
{
    BoundingBox bb;

    bb.position = beamStartPosition;
    bb.size = beamEndPosition - beamStartPosition;
    bb.MakeDimensionsPositive();
    bb.AddPadding(displayConstants.beamLineWidth);

    return bb;
}

void BeamGroup::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition)
{
    for (Beam& beam : beams)
    {
        beam.UpdateBoundingBox(displayConstants, parentPosition);
    }

    boundingBox.position = beamStartPosition + parentPosition;
    boundingBox.size = beamEndPosition - beamStartPosition;
    boundingBox.MakeDimensionsPositive();
    boundingBox.AddPadding(displayConstants.beamLineWidth);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void BeamGroup::CalculateAsPaged(const MusicDisplayConstants& displayConstants)
{
    if (notes.empty())
        return;

    if (notes[0] == nullptr)
    {
        LOGE("NOTE[0] IS NULL POINTER!!!!!");
        return;
    }

    if (notes[0]->noteSize == NoteSize::Grace)
        sizeFactor = displayConstants.graceNoteSize;
    else if (notes[0]->noteSize == NoteSize::Cue)
        sizeFactor = displayConstants.cueNoteSize;
    else
        sizeFactor = 1.0f;

    for (Beam& beam : beams)
    {
        beam.CalculateAsPaged(displayConstants, sizeFactor);
    }
}

float BeamGroup::GetPositionYOnBeam(float positionX) const
{
    if (beamStartPosition.x - beamEndPosition.x == 0)
    {
        LOGW("BeamGroup: Beam has no width");
        return 0.0f;
    }

    float slope = GetSlope(); // the slope of the beam

    float yIntercept = beamStartPosition.y - (slope * beamStartPosition.x); // solve for b (b = y - mx)

    return (slope * positionX) + yIntercept; // y = mx + b
}

float BeamGroup::GetSlope() const
{
    return (beamStartPosition.y - beamEndPosition.y) / (beamStartPosition.x - beamEndPosition.x);
}