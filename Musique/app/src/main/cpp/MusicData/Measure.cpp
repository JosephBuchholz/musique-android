#include "Measure.h"

float Measure::GetMiddleHeight(float staffLineCount, float lineSpacing)
{
    return (lineSpacing * (staffLineCount - 1));
}

float Measure::GetAboveHeight(float staffLineCount, float lineSpacing)
{
    return 50.0f;
}

float Measure::GetBelowHeight(float staffLineCount, float lineSpacing)
{
    return 50.0f;
}

float Measure::GetTotalHeight(float staffLineCount, float lineSpacing)
{
    return GetAboveHeight(staffLineCount, lineSpacing) +
    GetMiddleHeight(staffLineCount, lineSpacing) +
    GetBelowHeight(staffLineCount, lineSpacing);
}

float Measure::GetBeginningWidth() const {
    float width = 20.0f;

    if (showKeySignature)
    {
        width += 20.0f;
        width += 20.0f;
    }

    if (showTimeSignature)
    {
        width += 16.0f;
        width += 20.0f;
    }

    if (showClef)
    {
        width += 17.0f;
        width += 20.0f;
    }
    return width;
}

float Measure::CalculateMinWidth(float notesWidth) const {
    float width = GetBeginningWidth();

    /*for (auto& note : notes) {
        if (!note->isChord) {
            width += note->GetMinWidth();
        }
    }*/
    width += notesWidth;

    width += 10.0f;

    if (width == 0.0f) {
        width = 1.0f;
    }

    return width;
}

// update to use width
float Measure::GetNotePositionInMeasure(float width, int noteIndex) const {

    // if the note index is a chord it gets the root note of the chord
    while (true) {
        if (notes[noteIndex]->isChord && noteIndex != 0) {
            noteIndex -= 1;
            continue;
        }
        break;
    }

    float position = GetBeginningWidth();
    int i = 0;
    for (auto& note : notes) {

        if (i == noteIndex) {
            break;
        } else {
            if (!note->isChord) {
                position += note->GetMinWidth();
            }
        }

        i++;
    }

    return position;
}

// update to use width
float Measure::GetKeySignaturePositionInMeasure(float width) const {
    float position = GetClefPositionInMeasure(width) + 17.0f + 20.0f;

    return position;
}

// update to use width
float Measure::GetTimeSignaturePositionInMeasure(float width) const {

    float position = GetKeySignaturePositionInMeasure(width) + 20.0f + 20.0f;

    return position;
}

// update to use width
float Measure::GetClefPositionInMeasure(float width) const {
    float position = 10.0f;

    return position;
}

float Measure::GetPitchYPosition(Pitch pitch) const {
    float position = 0.0f;

    int y = GetLetterNumber(pitch.step) + pitch.octave*7; // the y position of pitch of the note

    int line = 5 - clef.line; // getting line starting from the top instead of the bottom
    if (clef.sign == "G" || clef.sign == "F" || clef.sign == "C") {
        int octave = 4; // the octave of the clef sign
        if (clef.sign == "F")
            octave = 3;
        int clefY = GetLetterNumber(clef.sign) + octave*7; // the y position of pitch of the clef

        position = (float)line - (y - clefY) * 0.5f; // the y position of the note on the staff
    }

    return position;
}

float Measure::CalculateNoteYPositionRelativeToMeasure(int noteIndex) const {
    Note* note = notes[noteIndex];
    return GetPitchYPosition(note->pitch);
}

int Measure::GetLetterNumber(const std::string& s) const {
    int num = 0;
    if (s == "C") {
        num = 0;
    } else if (s == "D") {
        num = 1;
    } else if (s == "E") {
        num = 2;
    } else if (s == "F") {
        num = 3;
    } else if (s == "G") {
        num = 4;
    } else if (s == "A") {
        num = 5;
    } else if (s == "B") {
        num = 6;
    }
    return num;
}

void Measure::CalculateWidthAsPaged()
{
    measureWidth = defaultMeasureWidth;
}

// from the bottom staff line
float Measure::GetClefLineYPosition(MusicDisplayConstants displayConstants, int lines) const
{
    float clefYPosition = 0.0f;
    float fromTopLine = 0.0f;
    int spaces = (lines - 1);

    if (clef.sign == "G" || clef.sign == "F" || clef.sign == "C") {
        clefYPosition = (float)(clef.line - 1) * displayConstants.lineSpacing;
        fromTopLine = (spaces*displayConstants.lineSpacing) - clefYPosition;
    }
    else if (clef.sign == "TAB")
    {
        clefYPosition = ((float)spaces/2.0f) * displayConstants.tabLineSpacing;
        fromTopLine = (spaces*displayConstants.tabLineSpacing) - clefYPosition;
    }
    else if (clef.sign == "percussion")
    {
        clefYPosition = ((float)spaces/2.0f) * displayConstants.lineSpacing;
        fromTopLine = (spaces*displayConstants.lineSpacing) - clefYPosition;
    }

    return fromTopLine;
}