#include "TrillMark.h"

#include "../../../RenderMeasurement.h"

void TrillMark::Render(RenderData& renderData, float notePositionX, float topStaffLinePositionY, Vec2<float> offset) const
{
    float offsetWidth = GetDimensions().x / 2.0f;
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), { position.x + notePositionX + offset.x - offsetWidth, position.y + topStaffLinePositionY + offset.y }, GetPaint()));
}

Vec2<float> TrillMark::GetDimensions() const
{
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, GetPaint()));
    return { bb.size.x, bb.size.y };
}

SMuFLID TrillMark::GetSMuFLID() const
{
    return SMuFLID::ornamentTrill;
}

Paint TrillMark::GetPaint() const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    return paint;
}

void TrillMark::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    Paint paint = GetPaint();
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), 0.0f, 0.0f, paint));

    float offsetWidth = bb.size.x / 2.0f;
    boundingBox.position.x = position.x + bb.position.x + parentPosition.x - offsetWidth;
    boundingBox.position.y = position.y + bb.position.y + parentPosition.y;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

    boundingBox.AddPadding(3.0f);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::NoHorizontal);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void TrillMark::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition)
{
    position.x = defaultPosition.x;
    position.y = -20.0f;
}