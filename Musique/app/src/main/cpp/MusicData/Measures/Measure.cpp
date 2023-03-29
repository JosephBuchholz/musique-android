#include "Measure.h"
#include "../../RenderMeasurement.h"

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
float Measure::GetKeySignaturePositionInMeasure(const System& system) const {
    float position;

    float clefWidth = MeausreClefWidth();

    if (showKeySignature || system.showKeySignature)
        position = GetClefPositionInMeasure(system) + 10.0f + clefWidth;
    else
        position = GetClefPositionInMeasure(system);

    return position;
}

// update to use width
float Measure::GetTimeSignaturePositionInMeasure(const System& system) const {
    float position;

    float keySignatureWidth = MeausreKeySignatureWidth();

    if (showTimeSignature || system.showTimeSignature)
        position = GetKeySignaturePositionInMeasure(system) + keySignatureWidth + 10.0f;
    else
        position = GetKeySignaturePositionInMeasure(system);

    return position;
}

// update to use width
float Measure::GetClefPositionInMeasure(const System& system) const {
    float position;

    if (showClef || system.showClef)
        position = 10.0f;
    else
        position = 0.0f;

    return position;
}

float Measure::GetPitchYPosition(Pitch pitch) const {
    float position = 0.0f;

    // transpose pitch
    // pitch.octave += transpose.octaveChange;

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

void Measure::CalculateAsPaged(const MusicDisplayConstants& displayConstants)
{
    measureWidth = defaultMeasureWidth;

    if (defStaffDistance == -1.0f) // use the value in displayConstants
        staffDistance = displayConstants.staffDistance;
    else
        staffDistance = defStaffDistance;
}

float Measure::MeausreClefWidth() const
{
    return RenderMeasurement::MeasureGlyph(Clef::GetClefSMuFLID(clef, 6)); // TODO: use actual staff lines
}

float Measure::MeausreKeySignatureWidth() const
{
    // TODO: make better to find the actual width and not the approx. width
    float width;

    if (type == MeasureType::Standard)
        width = (float)std::abs(keySignature.fifths) * RenderMeasurement::MeasureGlyph(SMuFLID::accidentalSharp);
    else
        width = 0.0f;

    return width;
}

float Measure::MeausreTimeSignatureWidth() const
{
    float topNumWidth = RenderMeasurement::MeasureGlyph(GetTimeSignatureSMuFLID(timeSignature.notes));
    float bottomNumWidth = RenderMeasurement::MeasureGlyph(GetTimeSignatureSMuFLID(timeSignature.noteType));
    return std::max(topNumWidth, bottomNumWidth);
}

float Measure::GetRepeatBarlinePositionX() const
{
    return GetBeginningWidth();
}

void Measure::UpdateBoundingBoxes(const Vec2<float>& measurePosition)
{
    for (Direction& direction : directions)
    {
        direction.UpdateBoundingBox(measurePosition);
    }
}