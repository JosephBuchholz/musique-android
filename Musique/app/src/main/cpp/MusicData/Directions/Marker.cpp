#include "Marker.h"

#include "../../RenderMeasurement.h"

void Marker::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position.x + measurePosition.x + offset.x, position.y + measurePosition.y + offset.y, paint));
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

void Marker::UpdateBoundingBox(Vec2<float> parentPosition)
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), 0.0f, 0.0f, paint));

    boundingBox.position.x = position.x + bb.position.x + parentPosition.x;
    boundingBox.position.y = position.y + bb.position.y + parentPosition.y;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

    boundingBox.AddPadding(3.0f);

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