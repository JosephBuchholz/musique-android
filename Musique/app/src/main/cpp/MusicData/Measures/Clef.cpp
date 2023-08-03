#include "Clef.h"

#include "../../RenderMeasurement.h"

void Clef::Render(RenderData& renderData, bool showSystemClef, float positionX, const MusicDisplayConstants& displayConstants, int lines, float offsetX, float offsetY) const
{
    // clef
    if (showClef || showSystemClef)
    {
        Paint paint;
        VisibleElement::ModifyPaint(paint);

        if (clefChanged)
            paint.glyphSizeFactor = renderData.displayConstants.clefChangeScale;

        SMuFLGlyph glyph = SMuFLGlyph(GetClefSMuFLID(*this, lines), { positionX + offsetX, position.y + offsetY }, paint);
        renderData.AddGlyph(glyph);
    }
}

// from the bottom staff line
float Clef::GetClefLinePositionY(const MusicDisplayConstants& displayConstants, int lines) const
{
    float clefYPosition = 0.0f;
    float fromTopLine = 0.0f;
    int spaces = (lines - 1);

    if (sign == "G" || sign == "F" || sign == "C") {
        clefYPosition = (float)(line - 1) * displayConstants.lineSpacing; // from the bottom staff line
        fromTopLine = (spaces*displayConstants.lineSpacing) - clefYPosition;
    }
    else if (sign == "TAB")
    {
        clefYPosition = ((float)spaces/2.0f) * displayConstants.tabLineSpacing;
        fromTopLine = (spaces*displayConstants.tabLineSpacing) - clefYPosition;
    }
    else if (sign == "percussion")
    {
        clefYPosition = ((float)spaces/2.0f) * displayConstants.lineSpacing;
        fromTopLine = (spaces*displayConstants.lineSpacing) - clefYPosition;
    }

    return fromTopLine;
}

SMuFLID Clef::GetClefSMuFLID(const Clef& clef, int staffLines)
{
    if (clef.sign == "G") {
        if (clef.octaveChange == 0)
            return SMuFLID::gClef;
        else if (clef.octaveChange == 1)
            return SMuFLID::gClef8va;
        else if (clef.octaveChange == -1)
            return SMuFLID::gClef8vb;
    } else if (clef.sign == "F") {
        if (clef.octaveChange == 0)
            return SMuFLID::fClef;
        else if (clef.octaveChange == 1)
            return SMuFLID::fClef8va;
        else if (clef.octaveChange == -1)
            return SMuFLID::fClef8vb;
    } else if (clef.sign == "C") {
        if (clef.octaveChange == 0)
            return SMuFLID::cClef;
        else if (clef.octaveChange == -1)
            return SMuFLID::cClef8vb;
    } else if (clef.sign == "TAB") {
        if (staffLines == 6)
            return SMuFLID::stringTabClef6;
        else
            return SMuFLID::stringTabClef4;
    } else if (clef.sign == "percussion") {
        return SMuFLID::unpitchedPercussionClef1;
    }

    return SMuFLID::None;
}

void Clef::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines)
{
    position.y = GetClefLinePositionY(displayConstants, staffLines);
}

void Clef::UpdateBoundingBox(const Vec2<float> &parentPosition, int staffLines, bool showSystemClef)
{
    if (showClef || showSystemClef)
    {
        Paint paint;
        VisibleElement::ModifyPaint(paint);
        BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetClefSMuFLID(*this, staffLines), { 0.0f, 0.0f }, paint));

        boundingBox.position.x = position.x + bb.position.x + parentPosition.x;
        boundingBox.position.y = position.y + bb.position.y + parentPosition.y;
        boundingBox.size.x = bb.size.x;
        boundingBox.size.y = bb.size.y;

        boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
        debugBoundingBox = boundingBox;
#endif
    }
}