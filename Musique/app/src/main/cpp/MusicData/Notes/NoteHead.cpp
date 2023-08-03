#include "NoteHead.h"

#include "../../RenderMeasurement.h"

void NoteHead::Render(RenderData& renderData, Vec2<float> position, bool centerHorizontally, Vec2<float> offset) const
{
    if (noteType == NoteType::Standard)
    {
        if (centerHorizontally)
            position.x -= GetDimensions(renderData.displayConstants).x / 2.0f;

        renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + offset, GetPaint(renderData.displayConstants)));
    }
    else if (noteType == NoteType::Tab)
    {
        //renderData.AddDebugDot(position.x + offset.x, position.y + offset.y);

        if (type == NoteHeadType::X)
        {
            //position.x -= GetDimensions(renderData.displayConstants).x / 2.0f;
            renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + offset, GetPaint(renderData.displayConstants)));
        }
        else
        {
            //position.x += GetDimensions(renderData.displayConstants).x / 2.0f;
            renderData.AddText(Text(ToString(fret), position + offset, GetPaint(renderData.displayConstants)));
        }

        //renderData.AddDebugDot(position.x + offset.x, position.y + offset.y);
    }
}

Vec2<float> NoteHead::GetDimensions(const MusicDisplayConstants& displayConstants) const
{
    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, GetPaint(displayConstants)));
    Paint paint = GetPaint(displayConstants);

    if (noteType == NoteType::Standard)
        bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, paint));
    else if (noteType == NoteType::Tab)
        bb = RenderMeasurement::GetTextBoundingBox(Text(ToString(fret), { 0.0f, 0.0f }, paint));

    return { bb.size.x, bb.size.y };
}

float NoteHead::GetCenterPositionX(const MusicDisplayConstants& displayConstants) const
{
    if (noteType == NoteType::Standard)
    {
        return GetNoteHeadWidth(displayConstants) / 2.0f;
    }
    else if (noteType == NoteType::Tab)
    {
        return GetNoteHeadWidth(displayConstants) / 2.0f;
    }

    return 0.0f;
}

SMuFLID NoteHead::GetSMuFLID() const
{
    switch (type)
    {
        case NoteHeadType::Normal:
        {
            switch (noteDuration)
            {
                case NoteValue::None: return SMuFLID::None;
                case NoteValue::Whole: return SMuFLID::noteheadWhole;
                case NoteValue::Half: return SMuFLID::noteheadHalf;

                case NoteValue::Quarter:
                case NoteValue::Eighth:
                case NoteValue::Sixteenth:
                case NoteValue::ThirtySecond: return SMuFLID::noteheadBlack;

                default: return SMuFLID::noteheadDoubleWhole;
            }
        }
        case NoteHeadType::Diamond: return SMuFLID::noteheadDiamondBlack;
        case NoteHeadType::Slash: return SMuFLID::noteheadSlashHorizontalEnds;
        case NoteHeadType::X: return SMuFLID::noteheadXBlack;
        case NoteHeadType::NoNoteHead:
        default: return SMuFLID::None;
    }
}

Paint NoteHead::GetPaint(const MusicDisplayConstants& displayConstants) const
{
    Paint paint;

    if (noteType == NoteType::Tab)
    {
        paint.textSize = 16.0f * sizeFactor;
        paint.isTablature = true;
    }

    VisibleElement::ModifyPaint(paint);
    paint.glyphSizeFactor = sizeFactor;

    return paint;
}

float NoteHead::GetNoteHeadWidth(const MusicDisplayConstants& displayConstants) const
{
    return GetDimensions(displayConstants).x;
}

BoundingBox NoteHead::GetBoundingBoxRelativeToParent(const MusicDisplayConstants& displayConstants) const
{
    BoundingBox bb;

    bb.size = GetDimensions(displayConstants);
    bb.position.x = 0.0f;
    bb.position.y = -(boundingBox.size.y / 2.0f);

    return bb;
}

void NoteHead::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition)
{
    boundingBox.size = GetDimensions(displayConstants);
    boundingBox.position.x = parentPosition.x;
    boundingBox.position.y = parentPosition.y - (boundingBox.size.y / 2.0f);

    /*if (type == NoteType::Tab)
    {
        boundingBox.position.x -= boundingBox.size.x / 2.0f;
    }*/

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void NoteHead::CalculateAsPaged(const MusicDisplayConstants& displayConstants, NoteSize noteSize)
{
    if (noteSize == NoteSize::Grace)
        sizeFactor = displayConstants.graceNoteSize;
    else if (noteSize == NoteSize::Cue)
        sizeFactor = displayConstants.cueNoteSize;
    else
        sizeFactor = 1.0f;
}