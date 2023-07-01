#include "Chord.h"
#include "../../Utils/Converters.h"

#include "../../RenderMeasurement.h"

void Chord::Render(RenderData& renderData, float measurePositionX, float measurePositionY, float offsetX, float offsetY) const
{
    // paint
    Paint paint = Paint();
    TextualElement::ModifyPaint(paint);

    // render
    renderData.AddText(Text(chordName.string, position.x + measurePositionX + offsetX, position.y + measurePositionY + offsetY, paint));

    if (chordDiagram)
        chordDiagram->Render(renderData, { position.x + measurePositionX, position.y + measurePositionY }, { offsetX, offsetY });

    /*std::vector<uint16_t> chars;
    chars.push_back((uint16_t)SMuFLID::accidentalFlat);

    for (auto& c : chordName.string)
    {
        if (c == '\0')
            break;
        chars.push_back(c);
    }

    std::vector<TextSpan> spans;
    Paint glyphPaint = Paint(color.color);
    glyphPaint.useMusicFont = true;
    glyphPaint.textSize = 40.0f;
    glyphPaint.align = Paint::Align::Left;

    Paint normalPaint = Paint(color.color);
    paint.isBold = true;
    paint.textSize = 16.0f;
    paint.align = Paint::Align::Left;

    spans.emplace_back(0, 1, glyphPaint);
    spans.emplace_back(1, 10, normalPaint);

    renderData.AddSpannableText(std::make_shared<SpannableText>(chars, positionX + measurePositionX + offsetX, positionY + measurePositionY + offsetY, spans, paint));*/
}

BoundingBox Chord::GetBoundingBoxRelativeToParent() const
{
    BoundingBox bb;

    Paint paint = Paint();
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    BoundingBox textBoundingBox = RenderMeasurement::GetTextBoundingBox(Text(chordName.string, position.x, position.y, paint));

    bb.position.x = textBoundingBox.position.x + position.x;
    bb.position.y = textBoundingBox.position.y + position.y;
    bb.position.x -= textBoundingBox.size.x / 2.0f;
    bb.position.y += textBoundingBox.size.y / 2.0f;
    bb.size.x = textBoundingBox.size.x;
    bb.size.y = textBoundingBox.size.y;

    if (chordDiagram)
    {
        bb = BoundingBox::CombineBoundingBoxes(bb, chordDiagram->GetBoundingBoxRelativeToParent());
    }

    return bb;
}

void Chord::UpdateBoundingBox(const Vec2<float> &parentPosition)
{
    boundingBox = GetBoundingBoxRelativeToParent();
    boundingBox.position += parentPosition;

    boundingBox.AddPadding();

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::NoHorizontal);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void Chord::CalculateChordName()
{
    chordName.string = rootPitch.step;

    if (rootPitch.alter == -1)
        chordName.string += "b";
    else if (rootPitch.alter == 1)
        chordName.string += "#";

    if (!harmonyTypeText.empty())
    {
        chordName.string += harmonyTypeText;
    }
    else
    {
        switch (harmonyType)
        {
            case HarmonyType::Major: break;
            case HarmonyType::MajorSeventh: chordName.string += "maj7"; break;
            case HarmonyType::Minor: chordName.string += "m"; break;
            case HarmonyType::MinorSeventh: chordName.string += "m7"; break;
            case HarmonyType::Dominant: chordName.string += "7"; break;
            case HarmonyType::Augmented: chordName.string += "aug"; break;
            case HarmonyType::Diminished: chordName.string += "dim"; break;
            case HarmonyType::HalfDiminished: chordName.string += "m7b5"; break;
            case HarmonyType::Power: chordName.string += "5"; break;
            case HarmonyType::SuspendedFourth: chordName.string += "sus"; break;
            case HarmonyType::SuspendedSecond: chordName.string += "sus2"; break;
            default: break;
        }
    }

    for (const ChordDegree& degree : degrees)
    {
        std::string sDegree = ToString((int) degree.degree.degree);
        if (degree.degreeType.type == DegreeType::Type::Add)
        {
            if (parentheses)
                chordName.string += "(" + sDegree + ")";
            else if (brackets)
                chordName.string += "[" + sDegree + "]";
            else
                chordName.string += "add" + sDegree;
        }
        else if (degree.degreeType.type == DegreeType::Type::Subtract)
        {
            chordName.string += "no" + sDegree;
        }
        else if (degree.degreeType.type == DegreeType::Type::Alter)
        {
            chordName.string += "alt" + sDegree;
        }
    }

    if (hasBassNote)
    {
        chordName.string += bassSeparator.string + bassPitch.step;
    }
}

void Chord::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY)
{
    position.x = defaultX;
    position.y = defaultY;

    if (chordDiagram)
        chordDiagram->CalculatePositionAsPaged(displayConstants, { 0.0f, 18.0f });
}