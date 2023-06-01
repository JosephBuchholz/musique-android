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

float Measure::GetBeginningWidth(System& system) const
{
    float width = 0.0f;

    width += GetTimeSignaturePositionInMeasure(system, GetKeySignaturePositionInMeasure(system, GetClefPositionInMeasure(system)));

    bool isBeginningMeasure = (system.beginningMeasureIndex == index);

    if (showTimeSignature || (system.showBeginningTimeSignature && isBeginningMeasure))
        width += MeausreTimeSignatureWidth();

    return width;
}

float Measure::CalculateMinWidth(float notesWidth) const
{
    //float width = GetBeginningWidth();
    float width = 20.0f;

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
float Measure::GetNotePositionInMeasure(float width, int noteIndex) const
{

    // if the note index is a chord it gets the root note of the chord
    while (true) {
        if (notes[noteIndex]->isChord && noteIndex != 0) {
            noteIndex -= 1;
            continue;
        }
        break;
    }

    //float position = GetBeginningWidth();
    float position = 20.0f;
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

float Measure::GetKeySignaturePositionInMeasure(const System& system, float clefPositionX) const
{

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

float Measure::GetTimeSignaturePositionInMeasure(const System& system, float keySignaturePositionX) const
{
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

float Measure::GetClefPositionInMeasure(const System& system) const
{
    float position = 5.0f;

    return position;
}

void Measure::RenderDebug(RenderData& renderData) const
{
    boundingBox.Render(renderData, (int)0x66FF0000);
    measureNumber.boundingBox.Render(renderData, (int)0x66FF0000);

    clef.boundingBox.Render(renderData, (int)0x66FF0000);

    for (auto note : notes)
    {
        note->RenderDebug(renderData);
    }

    for (auto noteChord : noteChords)
    {
        noteChord->RenderDebug(renderData);
    }

    for (const BeamGroup& beamGroup : beams)
    {
        beamGroup.RenderDebug(renderData);
    }

    for (const Direction& direction : directions)
    {
        direction.RenderDebug(renderData);
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

float Measure::GetPitchYPosition(Pitch pitch) const
{
    float position = 0.0f;

    // transpose pitch
    // pitch.octave += transpose.octaveChange;

    int y = GetLetterNumber(pitch.step) + pitch.octave*7; // the y position of pitch of the note

    int line = 5 - clef.line; // getting line starting from the top instead of the bottom
    if (clef.sign == "G" || clef.sign == "F" || clef.sign == "C") {
        int octave = 4; // the octave of the clef sign
        if (clef.sign == "F")
            octave = 3;

        octave += clef.octaveChange;

        int clefY = GetLetterNumber(clef.sign) + octave*7; // the y position of pitch of the clef

        position = (float)line - (y - clefY) * 0.5f; // the y position of the note on the staff
    }

    return position;
}

float Measure::CalculateNoteYPositionRelativeToMeasure(int noteIndex) const
{
    std::shared_ptr<Note> note = notes[noteIndex];
    return CalculateNoteYPositionRelativeToMeasure(note);
}

float Measure::CalculateNoteYPositionRelativeToMeasure(std::shared_ptr<Note> note) const
{
    return GetPitchYPosition(note->pitch);
}

int Measure::GetLetterNumber(const std::string& s) const
{
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

    // TODO: this measure data won't be finished until all measures have called CalculateAsPaged, thus this code needs to be moved
    measureDataItem->second.clefPositionX = std::max(GetClefPositionInMeasure(system), measureDataItem->second.clefPositionX);
    measureDataItem->second.keySignaturePositionX = std::max(GetKeySignaturePositionInMeasure(system, measureDataItem->second.clefPositionX), measureDataItem->second.keySignaturePositionX);
    measureDataItem->second.timeSignaturePositionX = std::max(GetTimeSignaturePositionInMeasure(system, measureDataItem->second.keySignaturePositionX), measureDataItem->second.timeSignaturePositionX);

    measureDataItem->second.measureWidth = measureWidth;
    measureDataItem->second.measureBeginningWidth = std::max(GetBeginningWidth(system), measureDataItem->second.measureBeginningWidth);

    measureDataItem->second.repeatBarlinePositionX = std::max(GetRepeatBarlinePositionX(system), measureDataItem->second.repeatBarlinePositionX);

    clef.CalculatePositionAsPaged(displayConstants, staffLines);
    keySignature.CalculatePositionAsPaged(displayConstants);
    timeSignature.CalculatePositionAsPaged(displayConstants);

    measureNumber.CalculatePositionAsPaged(displayConstants, { 0.0f, -15.0f });

    float lineSpacing;
    if (type == MeasureType::Tab)
        lineSpacing = displayConstants.tabLineSpacing;
    else
        lineSpacing = displayConstants.lineSpacing;

    if (isMeasureRepeat && measureRepeat != nullptr)
        measureRepeat->CalculatePositionAsPaged(displayConstants, measureDataItem->second, { measureWidth, GetMiddleHeight(staffLines, lineSpacing) });

    for (auto arpeggio : arpeggios)
    {
        if (arpeggio)
            arpeggio->CalculatePositionAsPaged(displayConstants, { 0.0f, 0.0f }, { 0.0f, 0.0f });
    }

    for (auto slur : slurs)
    {
        if (slur)
            slur->CalculatePositionAsPaged(displayConstants);
    }

    for (BeamGroup& beamGroup : beams)
    {
        beamGroup.CalculateAsPaged(displayConstants);
    }

    for (auto& barline : barlines)
    {
        float repeatBarlineOffset = 0.0f;

        if (startNewSystem)
        {
            repeatBarlineOffset = measureDataItem->second.repeatBarlinePositionX;
        }

        barline.CalculateAsPaged(displayConstants, measureWidth, repeatBarlineOffset);
    }
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

float Measure::GetRepeatBarlinePositionX(System& system) const
{
    float width = 0.0f;

    width += GetTimeSignaturePositionInMeasure(system, GetKeySignaturePositionInMeasure(system, GetClefPositionInMeasure(system)));

    bool isBeginningMeasure = (system.beginningMeasureIndex == index);

    if (showTimeSignature || (system.showBeginningTimeSignature && isBeginningMeasure))
        width += MeausreTimeSignatureWidth();

    return width;
}

void Measure::UpdateBoundingBoxes(const MusicDisplayConstants& displayConstants, const Vec2<float>& measurePosition, float measureHeight)
{
    boundingBox.position = measurePosition;
    boundingBox.size.x = measureWidth;
    boundingBox.size.y = measureHeight;

    measureNumber.UpdateBoundingBox(measurePosition);
    clef.UpdateBoundingBox(measurePosition, 5, true);

    for (auto note : notes)
    {
        note->UpdateBoundingBox(displayConstants, measurePosition);
    }

    for (auto noteChord : noteChords)
    {
        noteChord->UpdateBoundingBox(displayConstants, measurePosition);
    }

    for (BeamGroup& beamGroup : beams)
    {
        beamGroup.UpdateBoundingBox(displayConstants, measurePosition);
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