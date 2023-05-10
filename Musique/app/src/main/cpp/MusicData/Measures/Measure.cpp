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

    if (clef.showClef)
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

float Measure::GetKeySignaturePositionInMeasure(const System& system, float clefPositionX) const {

    float position;

    bool isBeginningMeasure = (system.beginningMeasureIndex == index);

    float clefWidth;

    if ((clef.showClef || (system.showBeginningClef && isBeginningMeasure)) && !clef.clefChanged) // showing the clef
        clefWidth = MeausreClefWidth();
    else // not showing the clef
        clefWidth = 0.0f;

    position = clefPositionX + clefWidth;

    return position;
}

float Measure::GetTimeSignaturePositionInMeasure(const System& system, float keySignaturePositionX) const {
    float position;
    bool isBeginningMeasure = (system.beginningMeasureIndex == index);

    float keySignatureWidth;

    if (showKeySignature || (system.showBeginningKeySignature && isBeginningMeasure)) // showing the key signature
        keySignatureWidth = MeausreKeySignatureWidth();
    else // not showing the key signature
        keySignatureWidth = 0.0f;

    position = keySignaturePositionX + keySignatureWidth;

    return position;
}

float Measure::GetClefPositionInMeasure(const System& system) const {
    float position = 5.0f;

    return position;
}

void Measure::RenderDebug(RenderData& renderData) const
{
    boundingBox.Render(renderData, (int)0x66FF0000);
    measureNumber.boundingBox.Render(renderData, (int)0x66FF0000);

    clef.boundingBox.Render(renderData, (int)0x66FF0000);

    for (auto* note : notes)
    {
        note->RenderDebug(renderData);
    }

    for (const Direction& direction : directions) {

        for (auto& words : direction.words)
        {
#if DEBUG_BOUNDING_BOXES
            words.debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif

            words.boundingBox.Render(renderData);
        }

        for (auto& rehearsal : direction.rehearsals)
        {
#if DEBUG_BOUNDING_BOXES
            rehearsal.debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif

            rehearsal.boundingBox.Render(renderData);
        }

        for (auto& dynamic : direction.dynamics)
        {
#if DEBUG_BOUNDING_BOXES
            dynamic.debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif

            dynamic.boundingBox.Render(renderData);
        }

        if (direction.dynamicWedge != nullptr)
        {
#if DEBUG_BOUNDING_BOXES
            direction.dynamicWedge->debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif

            direction.dynamicWedge->boundingBox.Render(renderData);
        }

        if (direction.bracketDirection != nullptr)
        {
#if DEBUG_BOUNDING_BOXES
            direction.bracketDirection->debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif

            direction.bracketDirection->boundingBox.Render(renderData);
        }

        if (direction.metronomeMark != nullptr)
        {
#if DEBUG_BOUNDING_BOXES
            direction.metronomeMark->debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif
            direction.metronomeMark->boundingBox.Render(renderData);
        }

    }

    for (const Chord& chord : chords)
    {
#if DEBUG_BOUNDING_BOXES
        chord.debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif

        chord.boundingBox.Render(renderData);

        /*if (chord.chordDiagram)
        {
#if DEBUG_BOUNDING_BOXES
            chord.chordDiagram->debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif
            chord.chordDiagram->boundingBox.Render(renderData);
        }*/
    }
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

void Measure::CalculateAsPaged(const MusicDisplayConstants& displayConstants, System& system, int staffLines)
{
    measureWidth = defaultMeasureWidth;

    if (defStaffDistance == -1.0f) // use the value in displayConstants
        staffDistance = displayConstants.staffDistance;
    else
        staffDistance = defStaffDistance;

    if (system.systemMeasureData.find(index) == system.systemMeasureData.end()) // the key does not exist
    {
        // so insert new measure data
        System::SystemMeasureData newSystemMeasureData;
        std::pair<int, System::SystemMeasureData> newPair(index, newSystemMeasureData);
        system.systemMeasureData.insert(newPair);
    }

    auto measureDataItem = system.systemMeasureData.find(index);

    measureDataItem->second.clefPositionX = std::max(GetClefPositionInMeasure(system), measureDataItem->second.clefPositionX);
    measureDataItem->second.keySignaturePositionX = std::max(GetKeySignaturePositionInMeasure(system, measureDataItem->second.clefPositionX), measureDataItem->second.keySignaturePositionX);
    measureDataItem->second.timeSignaturePositionX = std::max(GetTimeSignaturePositionInMeasure(system, measureDataItem->second.keySignaturePositionX), measureDataItem->second.timeSignaturePositionX);

    clef.CalculatePositionAsPaged(displayConstants, staffLines);
    keySignature.CalculatePositionAsPaged(displayConstants);
    timeSignature.CalculatePositionAsPaged(displayConstants);

    measureNumber.CalculatePositionAsPaged(displayConstants, { 0.0f, -15.0f });
}

float Measure::MeausreClefWidth() const
{
    // adding margins of 10.0f
    return RenderMeasurement::MeasureGlyph(Clef::GetClefSMuFLID(clef, 6)) + 10.0f; // TODO: use actual staff lines
}

float Measure::MeausreKeySignatureWidth() const
{
    // TODO: make better to find the actual width and not the approx. width
    float width;

    if (type == MeasureType::Standard)
        width = (float)std::abs(keySignature.fifths) * RenderMeasurement::MeasureGlyph(SMuFLID::accidentalSharp);
    else
        width = 0.0f;

    return width + 10.0f; // adding margins of 10.0f
}

float Measure::MeausreTimeSignatureWidth() const
{
    float topNumWidth = RenderMeasurement::MeasureGlyph(GetTimeSignatureSMuFLID(timeSignature.notes));
    float bottomNumWidth = RenderMeasurement::MeasureGlyph(GetTimeSignatureSMuFLID(timeSignature.noteType));
    return std::max(topNumWidth, bottomNumWidth) + 10.0f; // adding margins of 10.0f
}

float Measure::GetRepeatBarlinePositionX() const
{
    return GetBeginningWidth();
}

void Measure::UpdateBoundingBoxes(const Vec2<float>& measurePosition, float measureHeight)
{
    boundingBox.position = measurePosition;
    boundingBox.size.x = measureWidth;
    boundingBox.size.y = measureHeight;

    measureNumber.UpdateBoundingBox(measurePosition);
    clef.UpdateBoundingBox(measurePosition, 5, true);

    for (Note* note : notes)
    {
        note->UpdateBoundingBox(measurePosition);
    }

    for (Direction& direction : directions)
    {
        direction.UpdateBoundingBox(measurePosition);
    }

    for (Chord& chord : chords)
    {
        chord.UpdateBoundingBox(measurePosition);
    }
}