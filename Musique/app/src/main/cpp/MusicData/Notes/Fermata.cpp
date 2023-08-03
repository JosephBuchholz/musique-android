#include "Fermata.h"

#include "../../RenderMeasurement.h"

void Fermata::Render(RenderData& renderData, Vec2<float> parentPositionCenter, Vec2<float> offset) const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    float offsetWidth = (RenderMeasurement::MeasureGlyph(GetSMuFLID()) / 2.0f);
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), { position.x + parentPositionCenter.x + offset.x - offsetWidth, position.y + parentPositionCenter.y + offset.y }, paint));
}

void Fermata::RenderDebug(RenderData& renderData) const
{
#if DEBUG_BOUNDING_BOXES
    debugBoundingBox.Render(renderData, 0xFF00FF00);
#endif

    boundingBox.Render(renderData, 0xFF00FFFF);
}

SMuFLID Fermata::GetSMuFLID() const
{
    if (direction == Direction::Upright)
    {
        switch (shape)
        {
            case Shape::Normal: return SMuFLID::fermataAbove;
            case Shape::Angled: return SMuFLID::fermataShortAbove;
            case Shape::Square: return SMuFLID::fermataLongAbove;
            case Shape::DoubleAngled: return SMuFLID::fermataVeryShortAbove;
            case Shape::DoubleSquare: return SMuFLID::fermataVeryLongAbove;
            case Shape::DoubleDot: return SMuFLID::fermataLongHenzeAbove;
            case Shape::HalfCurve: return SMuFLID::fermataShortHenzeAbove;
            case Shape::Curlew: return SMuFLID::curlewSign;
            default: return SMuFLID::ErrorGlyph;
        }
    }
    else if (direction == Direction::Inverted)
    {
        switch (shape)
        {
            case Shape::Normal: return SMuFLID::fermataBelow;
            case Shape::Angled: return SMuFLID::fermataShortBelow;
            case Shape::Square: return SMuFLID::fermataLongBelow;
            case Shape::DoubleAngled: return SMuFLID::fermataVeryShortBelow;
            case Shape::DoubleSquare: return SMuFLID::fermataVeryLongBelow;
            case Shape::DoubleDot: return SMuFLID::fermataLongHenzeBelow;
            case Shape::HalfCurve: return SMuFLID::fermataShortHenzeBelow;
            case Shape::Curlew: return SMuFLID::curlewSign;
            default: return SMuFLID::ErrorGlyph;
        }
    }

    return SMuFLID::ErrorGlyph;
}

BoundingBox Fermata::GetBoundingBoxRelativeToParent() const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, paint));

    BoundingBox fermatBoundingBox;

    fermatBoundingBox.position.x = position.x + bb.position.x - (bb.size.x / 2.0f);
    fermatBoundingBox.position.y = position.y + bb.position.y;
    fermatBoundingBox.size.x = bb.size.x;
    fermatBoundingBox.size.y = bb.size.y;

    return fermatBoundingBox;
}

void Fermata::UpdateBoundingBox(const Vec2<float>& parentPositionCenter)
{
    boundingBox = GetBoundingBoxRelativeToParent();

    boundingBox.AddPadding(3.0f);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::NoHorizontal);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void Fermata::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPosition)
{
    position = defPosition;
}