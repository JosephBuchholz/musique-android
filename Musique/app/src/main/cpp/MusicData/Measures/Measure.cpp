#include "Measure.h"

#include "../../RenderMeasurement.h"
#include "../../Exceptions/Exceptions.h"

BoundingBox Measure::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int staffLineCount) const
{
    BoundingBox totalBoundingBox;

    float lineSpacing = displayConstants.lineSpacing;
    if (type == MeasureType::Tab)
        lineSpacing = displayConstants.tabLineSpacing;

    totalBoundingBox.size.y = float(staffLineCount - 1) * lineSpacing;
    totalBoundingBox.size.x = measureWidth;

    for (const auto& direction : directions)
    {
        totalBoundingBox = BoundingBox::CombineBoundingBoxes(totalBoundingBox, direction.GetBoundingBoxRelativeToMeasure());
    }

    for (const auto& note : notes)
    {
        totalBoundingBox = BoundingBox::CombineBoundingBoxes(totalBoundingBox, note->GetTotalBoundingBoxRelativeToMeasure(displayConstants));
    }

    for (const auto& noteChord : noteChords)
    {
        totalBoundingBox = BoundingBox::CombineBoundingBoxes(totalBoundingBox, noteChord->GetTotalBoundingBoxRelativeToMeasure(displayConstants));
    }

    for (const auto& beam : beams)
    {
        totalBoundingBox = BoundingBox::CombineBoundingBoxes(totalBoundingBox, beam.GetBoundingBoxRelativeToMeasure(displayConstants));
    }

    for (const auto& tuplet : tuplets)
    {
        totalBoundingBox = BoundingBox::CombineBoundingBoxes(totalBoundingBox, tuplet->GetBoundingBoxRelativeToParent());
    }

    for (const auto& chord : chords)
    {
        totalBoundingBox = BoundingBox::CombineBoundingBoxes(totalBoundingBox, chord.GetBoundingBoxRelativeToParent());
    }

    totalBoundingBox = BoundingBox::CombineBoundingBoxes(totalBoundingBox, measureNumber.GetBoundingBoxRelativeToMeasure(displayConstants));
    //totalBoundingBox = BoundingBox::CombineBoundingBoxes(totalBoundingBox, clef.GetBoundingBoxRelativeToMeasure(displayConstants));

    return totalBoundingBox;
}

float Measure::GetMiddleHeight(int staffLineCount, float lineSpacing)
{
    return (lineSpacing * float(staffLineCount - 1));
}

float Measure::GetAboveHeight(const MusicDisplayConstants& displayConstants, int staffLineCount) const
{
    BoundingBox bb = GetTotalBoundingBox(displayConstants, staffLineCount);
    float height = -bb.GetTop();
    return height;
}

float Measure::GetBelowHeight(int staffLineCount, float lineSpacing)
{
    return 50.0f;
}

float Measure::GetTotalHeight(const MusicDisplayConstants& displayConstants, int staffLineCount) const
{
    float height = GetTotalBoundingBox(displayConstants, staffLineCount).size.y;
    return height;
}

void Measure::Render(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition, float nextMeasurePositionX, const std::shared_ptr<System>& system, int staffLineCount, float staffLineSpacing, bool isTopMeasureLine, bool isLastMeasureInSystem, TablatureDisplayType tablatureDisplayType, bool isPartOfEnding) const
{
    if (startsMultiMeasureRest)
    {
        multiMeasureRestSymbol->Render(renderData, measurePosition);
    }

    RenderStaffLines(renderData, measurePosition, staffLineCount, staffLineSpacing);

    RenderBarlines(renderData, measurePosition.x, measurePosition.y, measureWidth, staffLineCount, staffLineSpacing, isTopMeasureLine, isPartOfEnding);

    RenderMeasureBeginning(renderData, measurePosition, system, staffLineCount, staffLineSpacing, isTopMeasureLine);

    // render directions
    if (!isMeasureRepeat)
    {
        for (const Direction& direction : directions)
        {
            direction.Render(renderData, measurePosition);
        }
    }

    if (isMeasureRepeat && measureRepeat != nullptr)
    {
        measureRepeat->Render(renderData, measurePosition);
    }

    if (!startsMultiMeasureRest && !isMeasureRepeat)
    {
        int noteIndex = 0;
        for (const auto& note : notes)
        {
            if (!note->isChord)
                note->Render(renderData, tablatureDisplayType, CalculateNoteYPositionRelativeToMeasure(noteIndex), staffLineCount, measurePosition, nextMeasurePositionX, staffLineSpacing);
            noteIndex++;
        }

        noteIndex = 0;
        for (const auto& noteChord : noteChords)
        {
            ASSERT(!noteChord->m_notes.empty());

            float rootNotePositionYRelativeToMeasure = CalculateNoteYPositionRelativeToMeasure(noteChord->m_notes[0]);
            float topNotePositionYRelativeToMeasure = CalculateNoteYPositionRelativeToMeasure(noteChord->m_notes[noteChord->m_notes.size()-1]);
            noteChord->Render(renderData, tablatureDisplayType, rootNotePositionYRelativeToMeasure, topNotePositionYRelativeToMeasure, staffLineCount, measurePosition, nextMeasurePositionX, staffLineSpacing);
        }

        // rendering note beams
        if (tablatureDisplayType == TablatureDisplayType::Full || type == Measure::MeasureType::Standard)
        {
            for (const BeamGroup &beamGroup : beams)
            {
                beamGroup.Render(renderData, measurePosition);
            }
        }

        // render all chords in this measure
        for (const Chord& chord : chords)
        {
            chord.Render(renderData, settings, measurePosition);
        }

        for (const auto& tuplet : tuplets)
        {
            tuplet->Render(renderData, measurePosition);
        }

        for (const auto& arpeggio : arpeggios)
        {
            float notePositionX = measurePosition.x;
            if (arpeggio)
                arpeggio->Render(renderData, notePositionX, measurePosition.y);
        }

        for (const auto& slur : slurs)
        {
            Vec2<float> startMeasurePosition = measurePosition;
            Vec2<float> endMeasurePosition = measurePosition;

            if (slur->startMeasureIndex != slur->endMeasureIndex)
            {
                endMeasurePosition.x = nextMeasurePositionX;
            }

            if (slur)
                slur->Render(renderData, startMeasurePosition, endMeasurePosition);
        }
    }
}

void Measure::RenderDebug(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition, float nextMeasurePositionX, const std::shared_ptr<System>& system, int staffLineCount, float staffLineSpacing, bool isTopMeasureLine, bool isLastMeasureInSystem, TablatureDisplayType tablatureDisplayType) const
{
    /*if (startsMultiMeasureRest)
    {
        multiMeasureRestSymbol->Render(renderData, measurePosition);
    }

    RenderStaffLines(renderData, measurePosition, staffLineCount, staffLineSpacing);

    RenderBarlines(renderData, measurePosition.x, measurePosition.y, measureWidth, staffLineCount, staffLineSpacing, isTopMeasureLine);

    RenderMeasureBeginning(renderData, measurePosition, system, staffLineCount, staffLineSpacing, isTopMeasureLine);*/

    // render directions
    if (!isMeasureRepeat)
    {
        for (const Direction& direction : directions)
        {
            direction.RenderDebug(renderData, measurePosition);
        }
    }


    /*if (isMeasureRepeat && measureRepeat != nullptr)
    {
        measureRepeat->Render(renderData, measurePosition);
    }*/

    if (!startsMultiMeasureRest && !isMeasureRepeat)
    {
        int noteIndex = 0;
        for (const auto& note : notes)
        {
            if (!note->isChord)
                note->RenderDebug(renderData, tablatureDisplayType, CalculateNoteYPositionRelativeToMeasure(noteIndex), staffLineCount, measurePosition, nextMeasurePositionX, staffLineSpacing);
            noteIndex++;
        }

        noteIndex = 0;
        for (const auto& noteChord : noteChords)
        {
            ASSERT(!noteChord->m_notes.empty());

            float rootNotePositionYRelativeToMeasure = CalculateNoteYPositionRelativeToMeasure(noteChord->m_notes[0]);
            float topNotePositionYRelativeToMeasure = CalculateNoteYPositionRelativeToMeasure(noteChord->m_notes[noteChord->m_notes.size()-1]);
            noteChord->RenderDebug(renderData, tablatureDisplayType, rootNotePositionYRelativeToMeasure, topNotePositionYRelativeToMeasure, staffLineCount, measurePosition, nextMeasurePositionX, staffLineSpacing);
        }

        for (const Chord& chord : chords)
        {
            chord.RenderDebug(renderData, settings, measurePosition);
        }

        // rendering note beams
        /*if (tablatureDisplayType == TablatureDisplayType::Full || type == Measure::MeasureType::Standard)
        {
            for (const BeamGroup &beamGroup : beams)
            {
                beamGroup.Render(renderData, measurePosition);
            }
        }

        for (auto tuplet : tuplets)
        {
            tuplet->Render(renderData, measurePosition);
        }

        for (auto arpeggio : arpeggios)
        {
            float notePositionX = measurePosition.x;
            if (arpeggio)
                arpeggio->Render(renderData, notePositionX, measurePosition.y);
        }

        for (auto slur : slurs)
        {
            Vec2<float> startMeasurePosition = measurePosition;
            Vec2<float> endMeasurePosition = measurePosition;

            if (slur->startMeasureIndex != slur->endMeasureIndex)
            {
                endMeasurePosition.x = nextMeasurePositionX;
            }

            if (slur)
                slur->Render(renderData, startMeasurePosition, endMeasurePosition);
        }*/
    }
}

void Measure::RenderStaffLines(RenderData& renderData, Vec2<float> measurePosition, int staffLineCount, float staffLineSpacing) const
{
    // staff lines
    Paint staffLinePaint = renderData.paints.barlinePaint;
    staffLinePaint.strokeWidth = renderData.displayConstants.staffLineWidth;

    for (int j = 0; j < staffLineCount; j++)
    {
        float endX = measurePosition.x + measureWidth;
        renderData.AddLine(Line({ measurePosition.x, (staffLineSpacing * (float)j) + measurePosition.y }, { endX, (staffLineSpacing * (float)j) + measurePosition.y }, staffLinePaint));
    }
}

void Measure::RenderMeasureBeginning(RenderData& renderData, Vec2<float> measurePosition, const std::shared_ptr<System>& system, int staffLineCount, float staffLineSpacing, bool isTopMeasureLine) const
{
    auto measureDataItem = system->systemMeasureData.find(index);

    if (measureDataItem != system->systemMeasureData.end())
    {
        bool displayTimeSignature = showTimeSignature || (system->showBeginningTimeSignature && startNewSystem);
        timeSignature.Render(renderData, displayTimeSignature, measureDataItem->second.timeSignaturePositionX + measurePosition.x, measurePosition.y, staffLineSpacing, staffLineCount);

        bool displayKeySignature = (showKeySignature || (system->showBeginningKeySignature && startNewSystem));
        keySignature.Render(renderData, displayKeySignature, measureDataItem->second.keySignaturePositionX + measurePosition.x, staffLineSpacing, staffLineCount, clef, 0.0f, measurePosition.y);

        bool showSystemClef = (system->showBeginningClef && startNewSystem);

        float clefPositionX;
        if (clef.clefChanged)
        {
            clefPositionX = measurePosition.x - 28.0f;
        }
        else
        {
            clefPositionX = measureDataItem->second.clefPositionX + measurePosition.x;
        }

        clef.Render(renderData, showSystemClef, clefPositionX, renderData.displayConstants, staffLineCount, 0.0f, measurePosition.y);
    }

    if (startNewSystem && isTopMeasureLine && measureNumber.GetNumber() != 1)
    {
        measureNumber.Render(renderData, measurePosition);
    }
}

void Measure::RenderBarlines(RenderData& renderData, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing, bool isTopStaff, bool isPartOfEnding) const
{
    float measureHeight = (lineSpacing * float(lineCount - 1));
    float barlineLeftPositionX = measurePositionX;
    float barlineRightPositionX = measurePositionX + measureWidth;
    float barlinePositionYTop = measurePositionY;
    float barlinePositionYBottom = measurePositionY + measureHeight;

    bool renderedRightBarline = false;

    for (const auto& barline : barlines)
    {
        if (barline.location == Barline::Location::Right)
            renderedRightBarline = true;

        barline.Render(renderData, { measurePositionX, measurePositionY }, measureHeight, lineSpacing, lineCount, isTopStaff, isPartOfEnding);
    }

    if (!renderedRightBarline)
        renderData.AddLine(Line({ barlineRightPositionX, barlinePositionYTop }, { barlineRightPositionX, barlinePositionYBottom }, renderData.paints.barlinePaint));

    // debug lines
    //renderData.AddLine(std::make_shared<Line>(barlineRightPositionX, barlinePositionYTop, barlineRightPositionX, barlinePositionYBottom, Paint(0xFFFF00FF)));
    //renderData.AddLine(std::make_shared<Line>(barlineLeftPositionX, barlinePositionYTop, barlineLeftPositionX, barlinePositionYBottom, Paint(0xFFFF00FF)));
}

float Measure::GetBeginningWidth(const std::shared_ptr<System>& system) const
{
    float width = 0.0f;

    width += GetTimeSignaturePositionInMeasure(system, GetKeySignaturePositionInMeasure(system, GetClefPositionInMeasure(system)));

    bool isBeginningMeasure = (system->beginningMeasureIndex == index);

    if (showTimeSignature || (system->showBeginningTimeSignature && isBeginningMeasure))
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

float Measure::GetKeySignaturePositionInMeasure(const std::shared_ptr<System>& system, float clefPositionX) const
{

    float position;

    bool isBeginningMeasure = (system->beginningMeasureIndex == index);

    float clefWidth;

    if ((clef.showClef || (system->showBeginningClef && isBeginningMeasure)) && !clef.clefChanged) // showing the clef
        clefWidth = MeausreClefWidth();
    else // not showing the clef
        clefWidth = 0.0f;

    position = clefPositionX + clefWidth;

    return position;
}

float Measure::GetTimeSignaturePositionInMeasure(const std::shared_ptr<System>& system, float keySignaturePositionX) const
{
    float position;
    bool isBeginningMeasure = (system->beginningMeasureIndex == index);

    float keySignatureWidth;

    if (showKeySignature || (system->showBeginningKeySignature && isBeginningMeasure)) // showing the key signature
        keySignatureWidth = MeausreKeySignatureWidth();
    else // not showing the key signature
        keySignatureWidth = 0.0f;

    position = keySignaturePositionX + keySignatureWidth;

    return position;
}

float Measure::GetClefPositionInMeasure(const std::shared_ptr<System>& system) const
{
    float position = 5.0f;

    return position;
}

void Measure::RenderDebug(RenderData& renderData) const
{
    boundingBox.Render(renderData, (int)0x66FF0000);

//#if DEBUG_BOUNDING_BOXES
//    debugBoundingBox.Render(renderData, (int)0xFF0000FF);
//#endif

    measureNumber.boundingBox.Render(renderData, (int)0x66FF0000);

    clef.boundingBox.Render(renderData, (int)0x66FF0000);

    for (const auto& note : notes)
    {
        note->RenderDebug(renderData);
    }

    for (const auto& noteChord : noteChords)
    {
        noteChord->RenderDebug(renderData);
    }

    for (const BeamGroup& beamGroup : beams)
    {
        beamGroup.RenderDebug(renderData);
    }

    for (const auto& tuplet : tuplets)
    {
        tuplet->RenderDebug(renderData);
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

float Measure::GetPitchYPosition(const Pitch& pitch) const
{
    float position = 0.0f;

    // transpose pitch
    // pitch.octave += transpose.octaveChange;

    int y = GetLetterNumber(DiatonicNoteToString(pitch.step)) + pitch.octave*7; // the y position of pitch of the note

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

float Measure::CalculateNoteYPositionRelativeToMeasure(const std::shared_ptr<Note>& note) const
{
    return GetPitchYPosition(note->pitch);
}

int Measure::GetLetterNumber(const std::string& s)
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

void Measure::CalculateAsPaged(const MusicDisplayConstants& displayConstants, const Settings& settings, const std::shared_ptr<System>& system, int staffLines)
{
    measureWidth = defaultMeasureWidth;

    if (defStaffDistance == -1.0f) // use the value in displayConstants
        staffDistance = displayConstants.staffDistance;
    else
        staffDistance = defStaffDistance;

    if (system->systemMeasureData.find(index) == system->systemMeasureData.end()) // the key does not exist
    {
        // so insert new measure data
        System::SystemMeasureData newSystemMeasureData;
        std::pair<int, System::SystemMeasureData> newPair(index, newSystemMeasureData);
        system->systemMeasureData.insert(newPair);
    }

    auto measureDataItem = system->systemMeasureData.find(index);

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

    for (const auto& arpeggio : arpeggios)
    {
        if (arpeggio)
            arpeggio->CalculatePositionAsPaged(displayConstants, { 0.0f, 0.0f }, { 0.0f, 0.0f });
    }

    for (const auto& slur : slurs)
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

    for (auto& chord : chords)
    {
        chord.CalculateChordName(settings);
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
    float topNumWidth = RenderMeasurement::MeasureGlyph(TimeSignature::GetTimeSignatureSMuFLID(timeSignature.notes));
    float bottomNumWidth = RenderMeasurement::MeasureGlyph(TimeSignature::GetTimeSignatureSMuFLID(timeSignature.noteType));
    return std::max(topNumWidth, bottomNumWidth) + 10.0f; // adding margins of 10.0f
}

float Measure::GetRepeatBarlinePositionX(const std::shared_ptr<System>& system) const
{
    float width = 0.0f;

    width += GetTimeSignaturePositionInMeasure(system, GetKeySignaturePositionInMeasure(system, GetClefPositionInMeasure(system)));

    bool isBeginningMeasure = (system->beginningMeasureIndex == index);

    if (showTimeSignature || (system->showBeginningTimeSignature && isBeginningMeasure))
        width += MeausreTimeSignatureWidth();

    return width;
}

void Measure::UpdateBoundingBoxes(const MusicDisplayConstants& displayConstants, const Vec2<float>& measurePosition, float measureHeight)
{
    boundingBox.position = measurePosition;
    boundingBox.size.x = measureWidth;
    boundingBox.size.y = measureHeight;
    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

/*#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = GetTotalBoundingBox(displayConstants, 5);
    debugBoundingBox.position += measurePosition;
#endif*/

    measureNumber.UpdateBoundingBox(measurePosition);

    if (!isMeasureRepeat)
    {
        clef.UpdateBoundingBox(measurePosition, 5, true);

        for (const auto& note: notes)
        {
            note->UpdateBoundingBox(displayConstants, measurePosition);
        }

        for (const auto& noteChord: noteChords)
        {
            noteChord->UpdateBoundingBox(displayConstants, measurePosition);
        }

        for (BeamGroup &beamGroup: beams)
        {
            beamGroup.UpdateBoundingBox(displayConstants, measurePosition);
        }

        for (const auto& tuplet : tuplets)
        {
            tuplet->UpdateBoundingBox(measurePosition);
        }

        for (Direction &direction: directions)
        {
            direction.UpdateBoundingBox(measurePosition);
        }

        for (Chord &chord: chords)
        {
            chord.UpdateBoundingBox(measurePosition);
        }
    }
}

void Measure::OnUpdate(const std::shared_ptr<Player>& player, bool isCurrentMeasure, int channel, float playLineBeatPosition, float previousPlayLineBeatPosition, float measureBeatPosition, float& currentTempo, SwingTempo& swingTempo, float& currentVelocity, uint32_t& capo)
{
    Transpose trans = transpose;
    trans.chromatic = (int)capo;

    if (isCurrentMeasure && playLineBeatPosition >= measureBeatPosition && playLineBeatPosition <= duration.duration + measureBeatPosition)
    {
        for (const auto& event : soundEvents)
        {
            if (playLineBeatPosition >= event->beatPosition + measureBeatPosition &&
                previousPlayLineBeatPosition <= event->beatPosition + measureBeatPosition)
            {
                uint32_t capoBefore = capo;
                event->ModifyTempo(currentTempo);
                event->ModifyVelocity(currentVelocity);
                event->ModifySwingTempo(swingTempo);
                //event->ModifyPizzicato();
                event->ModifyCapo(capo);
                LOGV_TAG("Measure.cpp", "event capo before: %d, after: %d", capoBefore, capo);
                LOGD_TAG("Measure.cpp", "Got sound event: velocity: %f, tempo: %f", currentVelocity, currentTempo);
            }
        }

        trans.chromatic = (int)capo; // in case it has been updated;

        for (const auto& noteChord : noteChords)
        {
            std::shared_ptr<Note> rootNote = noteChord->m_notes[0];

            float noteBeatPosition = rootNote->soundBeatPosition;
            float noteDuration = rootNote->soundDuration;

            if (swingTempo.swingType != SwingTempo::SwingType::Straight)
            {
                float first = swingTempo.first;
                float second = swingTempo.second;

                if (swingTempo.swingType == SwingTempo::SwingType::Eighth)
                {
                    float frac = (1.0f / (first + second));

                    if (IsOnBeatEighthNote(rootNote))
                    {
                        noteDuration = frac * first;
                    }
                    else if (IsOffBeatEighthNote(rootNote))
                    {
                        noteDuration = frac * second;
                        noteBeatPosition += (frac * first) - 0.5f;
                    }
                }
                else if (swingTempo.swingType == SwingTempo::SwingType::Sixteenth)
                {
                    float frac = (0.5f / (first + second));

                    if (IsOnBeatSixteenthNote(rootNote))
                    {
                        noteDuration = frac * first;
                    }
                    else if (IsOffBeatSixteenthNote(rootNote))
                    {
                        noteDuration = frac * second;
                        noteBeatPosition += (frac * first) - 0.25f;
                    }
                }
            }

            if (playLineBeatPosition >= noteBeatPosition + measureBeatPosition &&
                previousPlayLineBeatPosition <= noteBeatPosition + measureBeatPosition)
            {
                if (!rootNote->isPlaying)
                {
                    noteChord->OnPlay(player, trans, channel, currentVelocity);
                }
            }
            else if (rootNote->isPlaying &&
                     !(playLineBeatPosition <= noteBeatPosition + noteDuration + measureBeatPosition)) // note is playing but/and the play line has passed the end of the note
            { // so stop the note
                noteChord->OnStop(player, trans, channel, currentVelocity);
            }
            else if (rootNote->isPlaying)
            {
                noteChord->OnUpdate(player, trans, channel, currentVelocity, playLineBeatPosition - (noteBeatPosition + measureBeatPosition), previousPlayLineBeatPosition - (noteBeatPosition + measureBeatPosition));
            }
        }

        int noteIndex = 0;
        for (const auto& note : notes)
        {
            float noteBeatPosition = note->soundBeatPosition;
            float noteDuration = note->soundDuration;

            if (swingTempo.swingType != SwingTempo::SwingType::Straight)
            {
                float first = swingTempo.first;
                float second = swingTempo.second;

                if (swingTempo.swingType == SwingTempo::SwingType::Eighth)
                {
                    float frac = (1.0f / (first + second));

                    if (IsOnBeatEighthNote(note))
                    {
                        noteDuration = frac * first;
                    }
                    else if (IsOffBeatEighthNote(note))
                    {
                        noteDuration = frac * second;
                        noteBeatPosition += (frac * first) - 0.5f;
                    }
                }
                else if (swingTempo.swingType == SwingTempo::SwingType::Sixteenth)
                {
                    float frac = (0.5f / (first + second));

                    if (IsOnBeatSixteenthNote(note))
                    {
                        noteDuration = frac * first;
                    }
                    else if (IsOffBeatSixteenthNote(note))
                    {
                        noteDuration = frac * second;
                        noteBeatPosition += (frac * first) - 0.25f;
                    }
                }
            }

            if (playLineBeatPosition >= noteBeatPosition + measureBeatPosition &&
                previousPlayLineBeatPosition <= noteBeatPosition + measureBeatPosition)
            {
                if (!note->isPlaying)
                {
                    note->OnPlay(player, trans, channel, currentVelocity);
                }
            }
            else if (note->isPlaying &&
                     !(playLineBeatPosition <= noteBeatPosition + noteDuration + measureBeatPosition)) // note is playing but/and the play line has passed the end of the note
            { // so stop the note
                note->OnStop(player, trans, channel, currentVelocity);
            }
            else if (note->isPlaying)
            {
                note->OnUpdate(player, trans, channel, currentVelocity, playLineBeatPosition - (noteBeatPosition + measureBeatPosition), previousPlayLineBeatPosition - (noteBeatPosition + measureBeatPosition));
            }

            noteIndex++;
        }
    }
    else
    {
        for (const auto& noteChord : noteChords)
        {
            if (noteChord->m_notes[0]->isPlaying)
            {
                noteChord->OnStop(player, trans, channel, currentVelocity);
            }
        }

        for (const auto& note : notes)
        {
            if (note->isPlaying)
            {
                note->OnStop(player, trans, channel, currentVelocity);
            }
        }
    }
}

bool Measure::IsRepeatBackward() const
{
    for (const auto& barline : barlines)
    {
        if (barline.isRepeatBarLine)
        {
            if (barline.facing == Barline::Direction::Backward)
            {
                return true;
            }
        }
    }

    return false;
}

bool Measure::IsRepeatForward() const
{
    for (const auto& barline : barlines)
    {
        if (barline.isRepeatBarLine)
        {
            if (barline.facing == Barline::Direction::Forward)
            {
                return true;
            }
        }
    }

    return false;
}

int Measure::GetRepeatCount() const
{
    for (const auto& barline : barlines)
    {
        if (barline.isRepeatBarLine)
        {
            if (barline.facing == Barline::Direction::Backward)
            {
                return barline.repeatCount;
            }
        }
    }

    return 1;
}

bool Measure::IsOnBeatEighthNote(const std::shared_ptr<Note>& note) const
{
    if (note->durationType == NoteValue::Eighth && note->duration.duration == 0.5f)
    {
        float beat = 1.0f;
        for (int i = 0; i < timeSignature.notes; i++)
        {
            if (note->beatPosition == beat * (float)i)
                return true;
        }
    }

    return false;
}

bool Measure::IsOffBeatEighthNote(const std::shared_ptr<Note>& note) const
{
    if (note->durationType == NoteValue::Eighth && note->duration.duration == 0.5f)
    {
        float beat = 1.0f;
        float offBeatOffset = 0.5f;
        for (int i = 0; i < timeSignature.notes; i++)
        {
            if (note->beatPosition == (beat * (float)i) + offBeatOffset)
                return true;
        }
    }

    return false;
}

bool Measure::IsOnBeatSixteenthNote(const std::shared_ptr<Note>& note) const
{
    if (note->durationType == NoteValue::Sixteenth && note->duration.duration == 0.25f)
    {
        float beat = 0.5f;
        for (int i = 0; i < (timeSignature.notes * 2); i++)
        {
            if (note->beatPosition == beat * (float)i)
                return true;
        }
    }

    return false;
}

bool Measure::IsOffBeatSixteenthNote(const std::shared_ptr<Note>& note) const
{
    if (note->durationType == NoteValue::Sixteenth && note->duration.duration == 0.25f)
    {
        float beat = 0.5f;
        float offBeatOffset = 0.25f;
        for (int i = 0; i < (timeSignature.notes * 2); i++)
        {
            if (note->beatPosition == (beat * (float)i) + offBeatOffset)
                return true;
        }
    }

    return false;
}

void Measure::InitBeatPosition(float measureBeatPosition)
{
    beatPosition = measureBeatPosition;

    for (const auto& note : notes)
    {
        note->beatPositionInSong = note->beatPosition + measureBeatPosition;
    }

    for (Chord& chord : chords)
    {
        chord.beatPositionInSong = chord.beatPosition + measureBeatPosition;
    }

    for (Direction& direction : directions)
    {
        direction.beatPositionInSong = direction.beatPosition + measureBeatPosition;

        if (direction.dynamicWedge != nullptr)
        {
            direction.dynamicWedge->beatPositionInSongStart = direction.dynamicWedge->beatPositionStart + measureBeatPosition;
            direction.dynamicWedge->beatPositionInSongEnd = direction.dynamicWedge->beatPositionEnd + measureBeatPosition;
        }

        if (direction.bracketDirection != nullptr)
        {
            direction.bracketDirection->beatPositionInSongStart = direction.bracketDirection->beatPositionStart + measureBeatPosition;
            direction.bracketDirection->beatPositionInSongEnd = direction.bracketDirection->beatPositionEnd + measureBeatPosition;
        }
    }
}

bool Measure::UsesOpenStrings() const
{
    for (const auto& note : notes)
    {
        if (note->type == NoteType::Tab && note->fret == 0 && !note->isRest)
            return true;
    }

    for (const auto& noteChord : noteChords)
    {
        for (const auto& note : noteChord->m_notes)
        {
            if (note->type == NoteType::Tab && note->fret == 0 && !note->isRest)
                return true;
        }
    }

    return false;
}

void Measure::OnTranspose(const TranspositionRequest& transposeRequest)
{
    MusicalKey currentKey;
    currentKey.root = MusicalKey::GetKeyFromFifths(keySignature.fifths);

    for (const auto& note : notes)
    {
        note->OnTranspose(transposeRequest, currentKey);
    }

    for (const auto& noteChord : noteChords)
    {
        noteChord->OnTranspose(transposeRequest, currentKey);
    }

    for (auto& chord : chords)
    {
       chord.OnTranspose(transposeRequest, currentKey);
    }

    for (auto& direction : directions)
    {
        direction.OnTranspose(transposeRequest);
    }

    int interval = transposeRequest.GetInterval(); // interval between the keys
    int diatonicInterval = transposeRequest.GetDiatonicInterval(); // diatonic interval between the keys
    MusicalKey newKey = currentKey;
    if (currentKey.root == transposeRequest.originalKey.root)
        newKey = transposeRequest.key;
    else
        newKey.AddInterval(interval, diatonicInterval);

    //LOGW("interval: %d, fifths: %d, currentKey.root: %d, currentKey.root.fifths: %d, transpose.oKey.root: %d, toKey.fifths: %d, tKey.root: %d, tKey.fifths: %d, newKey.root: %d, newKey.fifths: %d",
    //     interval, keySignature.fifths, currentKey.root, currentKey.GetFifths(), transposeRequest.originalKey.root, transposeRequest.originalKey.GetFifths(), transposeRequest.key.root, transposeRequest.key.GetFifths(), newKey.root, newKey.GetFifths());

    keySignature.fifths = newKey.GetFifths();
}