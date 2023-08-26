#include "Marker.h"

#include "../../RenderMeasurement.h"

void Marker::Render(RenderData& renderData, Vec2<float> measurePosition) const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + measurePosition, paint));
}

void Marker::RenderDebug(RenderData& renderData, Vec2<float> measurePosition) const
{
    BoundingBox bb = GetBoundingBoxRelativeToParent();

    bb.position += measurePosition;

    bb.Render(renderData, 0xFFFF0000);
}

SMuFLID Marker::GetSMuFLID() const
{
    switch (type)
    {
        case MarkerType::Coda: return SMuFLID::coda;
        case MarkerType::Segno: return SMuFLID::segno;
        default: return SMuFLID::None;
    }
}

Vec2<float> Marker::GetDimensions() const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), 0.0f, 0.0f, paint));

    bb.position.x = position.x + bb.position.x;
    bb.position.y = position.y + bb.position.y;

    return bb.size;
}

BoundingBox Marker::GetBoundingBoxRelativeToParent() const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), 0.0f, 0.0f, paint));

    bb.position.x = position.x + bb.position.x;
    bb.position.y = position.y + bb.position.y;

    return bb;
}

void Marker::UpdateBoundingBox(Vec2<float> parentPosition)
{
    boundingBox = GetBoundingBoxRelativeToParent();
    boundingBox.position += parentPosition;

    boundingBox.AddPadding(3.0f);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::None);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void Marker::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPosition)
{
    position = defPosition;

    if (defaultPosition.x != INVALID_FLOAT_VALUE)
    {
        position.x = defaultPosition.x;

        if (relativePosition.x != INVALID_FLOAT_VALUE)
            position.x += relativePosition.x;
    }

    if (defaultPosition.y != INVALID_FLOAT_VALUE)
    {
        position.y = -defaultPosition.y;

        if (relativePosition.y != INVALID_FLOAT_VALUE)
            position.y -= relativePosition.y;
    }
}