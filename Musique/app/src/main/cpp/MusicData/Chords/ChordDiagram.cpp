#include "ChordDiagram.h"

#include <unordered_map>

void ChordDiagram::Render(RenderData& renderData, Vec2<float> chordPosition) const
{
    float nutPositionY = position.y + chordPosition.y;
    float positionX = position.x + chordPosition.x - (width / 2.0f);

    // render string lines
    float stringGap = width / (float)(strings - 1);
    float stringPositionX = positionX;
    for (int i = 0; i < strings; i++)
    {
        renderData.AddLine(Line(stringPositionX, nutPositionY, stringPositionX, nutPositionY + height, Paint(0xFF000000)));

        stringPositionX += stringGap;
    }

    // render the nut
    Paint nutPaint = Paint(color.color);

    if (firstFret == 1) // show nut
        nutPaint.strokeWidth = 2.5f;
    else
        ; // do nothing

    renderData.AddLine(Line(positionX, nutPositionY, positionX + width, nutPositionY, nutPaint));

    // render fret lines
    float fretGap = height / (float)frets;
    float fretPositionY = nutPositionY + fretGap;
    for (int i = 1; i < frets + 1; i++)
    {
        renderData.AddLine(Line(positionX, fretPositionY, positionX + width, fretPositionY, Paint(0xFF000000)));

        fretPositionY += fretGap;
    }

    // render the notes

    std::unordered_map<int, bool> usedStrings;

    for (int i = 1; i < (strings + 1); i++)
        usedStrings[i] = false;

    for (auto note : notes)
    {
        if (note.fret == 0)
            renderData.AddGlyph(SMuFLGlyph(SMuFLID::fretboardO, positionX + ((strings - note.string) * stringGap), nutPositionY + (note.fret * fretGap) - (fretGap / 2.0f), Paint(color.color)));
        else
            renderData.AddGlyph(SMuFLGlyph(SMuFLID::fretboardFilledCircle, positionX + ((strings - note.string) * stringGap), nutPositionY + (note.fret * fretGap) - (fretGap / 2.0f), Paint(color.color)));

        usedStrings[(int)note.string] = true;
    }

    // render the x's
    for (int i = 1; i < (strings + 1); i++)
    {
        if (!usedStrings[i])
        {
            renderData.AddGlyph(SMuFLGlyph(SMuFLID::fretboardX, positionX + ((strings - i) * stringGap), nutPositionY - (fretGap / 2.0f), Paint(color.color)));
        }
    }

    LOGD("CHORD DIAGRAMS: strings: %d,  frets: %d, firstFret: %d, notesSize: %d", strings, frets, firstFret, notes.size());
}

BoundingBox ChordDiagram::GetBoundingBoxRelativeToParent() const
{
    float nutPositionY = position.y;
    float positionX = position.x - (width / 2.0f);

    BoundingBox bb;

    bb.size.x = width;
    bb.size.y = height;

    bb.position.x = positionX;
    bb.position.y = nutPositionY;

    return bb;
}

void ChordDiagram::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    boundingBox = GetBoundingBoxRelativeToParent();
    boundingBox.position += parentPosition;

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void ChordDiagram::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition)
{
    position = defaultPosition;
}