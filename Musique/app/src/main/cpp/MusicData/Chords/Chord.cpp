#include "Chord.h"
#include "../../Utils/Converters.h"

void Chord::Render(RenderData& renderData, float measurePositionX, float measurePositionY, float offsetX, float offsetY) const
{
    // paint
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    // render
    renderData.AddText(Text(chordName.string, positionX + measurePositionX + offsetX, positionY + measurePositionY + offsetY, Paint(color.color, paint)));
}

void Chord::UpdateBoundingBox(const Vec2<float> &parentPosition)
{
    Paint paint = Paint();
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize * 2.0f;
    bb.size.x = (paint.textSize * (float)chordName.string.size()) + (paint.textSize * 2.0f);
    bb.size.y = paint.textSize * 2.0f;

    boundingBox.position.x = bb.position.x + positionX + parentPosition.x;
    boundingBox.position.y = bb.position.y + positionY + parentPosition.y;
    boundingBox.position.x -= bb.size.x / 2.0f;
    boundingBox.position.y += bb.size.y / 2.0f;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void Chord::CalculateChordName()
{
    chordName.string = rootPitch.step;

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
    positionX = defaultX;
    positionY = defaultY;
}