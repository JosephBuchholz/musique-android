#include "Song.h"
#include "SongData.h"
#include "../Debuging/AndroidDebug.h"
#include "../Utils/Converters.h"

#include <algorithm>

#include "../Exceptions/Exceptions.h"

Song::Song()
{

}

Song::~Song()
{
    LOGD("deconstructing song data");
    m_MeasureWidths.clear();
    m_MeasureBeginWidths.clear();
    m_MinNoteData.clear();
    LOGD("done deconstructing song data");
}

std::shared_ptr<Instrument> Song::GetInstrument(const std::string& id) const {
    for (auto& inst : instruments) {
        if (inst->id == id) {
            return inst;
        }
    }

    return nullptr;
}

void Song::OnLayoutChanged(Settings::MusicLayout layout)
{
    settings.musicLayout = layout;
    OnUpdate();
}

void Song::OnUpdate()
{
    LOGD("updating song data");

    songData.instrumentInfos.clear();
    for (auto instrument : instruments) {
        InstrumentInfo instInfo = InstrumentInfo();
        instInfo.name = instrument->name.string;
        instInfo.volume = 100;
        instInfo.visible = true;
        songData.instrumentInfos.push_back(instInfo);
    }

    // tie calculations
    /*for (auto instrument : instruments)
    {
        for (auto staff : instrument->staves)
        {
            std::vector<std::shared_ptr<Note>> tiedStartNotes;

            int measureIndex = 0;
            for (auto measure : staff->measures)
            {
                int noteIndex = 0;
                for (auto note : measure->notes)
                {
                    //LOGV("s: %d", tiedStartNotes.size());
                    if (note->tie.type == NoteTie::TieType::Start)
                    {
                        //LOGW("Tie start: s: %d", tiedStartNotes.size());
                        tiedStartNotes.push_back(note);
                    }
                    else if (note->tie.type == NoteTie::TieType::Stop)
                    {
                        if (!tiedStartNotes.empty())
                        {
                            //LOGW("Tie end: s: %d", tiedStartNotes.size());

                            tiedStartNotes[0]->tie.tiedNote = note;
                            note->tie.tiedNote = tiedStartNotes[0];

                            CurveOrientation orientation;
                            AboveBelowType placement;

                            if (tiedStartNotes[0]->noteStem.stemType == NoteStem::StemType::Up)
                            {
                                orientation = CurveOrientation::Under;
                                placement = AboveBelowType::Below;
                            }
                            else
                            {
                                orientation = CurveOrientation::Over;
                                placement = AboveBelowType::Above;
                            }

                            tiedStartNotes[0]->tie.orientation = orientation;
                            note->tie.orientation = orientation;

                            tiedStartNotes[0]->tie.placement = placement;
                            note->tie.placement = placement;

                            tiedStartNotes.erase(tiedStartNotes.begin());
                        }
                    }

                    noteIndex++;
                }

                measureIndex++;
            }
        }
    }*/

    // beam calculations
    for (auto instrument : instruments)
    {
        for (auto staff : instrument->staves)
        {
            int measureIndex = 0;
            for (auto measure : staff->measures)
            {
                int noteIndex = 0;
                const int maxBeamLevel = 4; // TODO: probably should be fixed
                std::array<Beam, maxBeamLevel> beams; // temp to store/keep track of beams
                BeamGroup beamGroup; // temp to store/keep track of a beam group

                bool beamGroupIsDone = false;

                for (auto note : measure->notes)
                {
                    for (const NoteBeamData& noteBeamData : note->beamData)
                    {
                        if (noteBeamData.beamType == NoteBeamData::BeamType::Begin)
                        {
                            if (noteBeamData.beamLevel == 1) // start a new beam group
                            {
                                beamGroup = BeamGroup();

                                if (note->noteStem->stemType == NoteStem::StemType::Up)
                                    beamGroup.isAboveNote = true;
                            }

                            Beam beam = Beam();
                            beam.beamLevel = noteBeamData.beamLevel;
                            beam.notes.push_back(note);
                            if (noteBeamData.beamLevel == 1)
                                beamGroup.notes.push_back(note);
                            beam.beamType = Beam::BeamType::Normal;

                            beams[noteBeamData.beamLevel - 1] = beam;
                        }
                        else if (noteBeamData.beamType == NoteBeamData::BeamType::End)
                        {
                            beams[noteBeamData.beamLevel - 1].notes.push_back(note);
                            if (noteBeamData.beamLevel == 1)
                                beamGroup.notes.push_back(note);

                            beamGroup.beams.push_back(beams[noteBeamData.beamLevel - 1]);

                            if (noteBeamData.beamLevel == 1) // the end of this beam group
                                beamGroupIsDone = true;
                        }
                        else if (noteBeamData.beamType == NoteBeamData::BeamType::Continue)
                        {
                            beams[noteBeamData.beamLevel - 1].notes.push_back(note);
                            if (noteBeamData.beamLevel == 1)
                                beamGroup.notes.push_back(note);
                        }
                        else if (noteBeamData.beamType == NoteBeamData::BeamType::ForwardHook)
                        {
                            Beam beam = Beam();
                            beam.beamLevel = noteBeamData.beamLevel;
                            beam.notes.push_back(note);
                            beam.beamType = Beam::BeamType::ForwardHook;

                            beamGroup.beams.push_back(beam);
                        }
                        else if (noteBeamData.beamType == NoteBeamData::BeamType::BackwardHook)
                        {
                            Beam beam = Beam();
                            beam.beamLevel = noteBeamData.beamLevel;
                            beam.notes.push_back(note);
                            beam.beamType = Beam::BeamType::BackwardHook;

                            beamGroup.beams.push_back(beam);
                        }
                        else
                        {
                            LOGW("Beam type is not supported yet.");
                        }
                    }

                    if (beamGroupIsDone)
                    {
                        measure->beams.push_back(beamGroup);
                        beamGroupIsDone = false;
                    }

                    noteIndex++;
                }

                measureIndex++;
            }
        }
    }

    // beat positions
    for (auto instrument : instruments) {
        for (auto staff : instrument->staves) {
            totalBeatWidth = staff->GetTotalBeatWidth();
            int measureIndex = 0;
            for (auto measure : staff->measures) {
                measure->beatPosition = staff->GetMeasureBeatPosition(measureIndex);
                /*int minWidth = measure->CalculateMinWidth();

                if ((int)m_MeasureWidths.size() - 1 >= measureIndex) { // if there is all ready a width at measureNumber
                    if (m_MeasureWidths[measureIndex] < minWidth) { // if the width of this measure is bigger than the widths of the previous measures
                        m_MeasureWidths[measureIndex] = minWidth;
                    }
                } else { // else push back width
                    m_MeasureWidths.push_back(minWidth);
                }*/

                measureIndex++;
            }
        }
    }

    CalculateNoteBeatPositionsInSong();

    if (settings.musicLayout == Settings::MusicLayout::Paged)
    {
        //m_MinNoteData.clear();
        m_MeasureWidths.clear();
        m_MeasureBeginWidths.clear();
        LOGI("Paged");

        for (Credit& credit : credits)
        {
            credit.CalculatePositionAsPaged();
        }

        for (auto ending : endings)
        {
            if (ending)
                ending->CalculatePositionAsPaged(displayConstants);
        }

        // calculate positions for notes and measures
        for (auto instrument : instruments)
        {
            if (instrument->instrumentBracket)
                instrument->instrumentBracket->CalculateAsPaged(displayConstants, -15.0f);

            for (auto staff : instrument->staves)
            {
                staff->CalculateAsPaged(displayConstants);

                int measureIndex = 0;
                int systemIndex = -1;
                for (auto measure : staff->measures)
                {
                    if (measure->startNewSystem)
                        systemIndex++;

                    measure->CalculateAsPaged(displayConstants, systems[systemIndex], staff->lines);
                    if (measureIndex >= m_MeasureWidths.size())
                    {
                        m_MeasureBeginWidths.push_back(0.0f);
                        m_MeasureWidths.push_back(measure->measureWidth);
                    }

                    if (staff->type == Staff::StaffType::Tab)
                        measure->showKeySignature = false;

                    //m_MeasureBeginWidths[measureIndex] = std::max(measure->GetBeginningWidth(), m_MeasureBeginWidths[measureIndex]);

                    /*systems[systemIndex].clefPositionX = std::max(measure->GetClefPositionInMeasure(systems[systemIndex]), systems[systemIndex].clefPositionX);
                    systems[systemIndex].keySignaturePositionX = std::max(measure->GetKeySignaturePositionInMeasure(systems[systemIndex], systems[systemIndex].clefPositionX), systems[systemIndex].keySignaturePositionX);
                    systems[systemIndex].timeSignaturePositionX = std::max(measure->GetTimeSignaturePositionInMeasure(systems[systemIndex], systems[systemIndex].keySignaturePositionX), systems[systemIndex].timeSignaturePositionX);*/

                    int noteIndex = 0;
                    for (auto note : measure->notes)
                    {
                        /*if (note->isRest && note->type == NoteType::Tab && softwareName == "MuseScore" && softwareMajorVersion == 4) // musescore only
                        {
                            note->defaultPosition.y = INVALID_FLOAT_VALUE;
                        }*/

                        if (note->isRest)
                        {
                            note->defaultPosition.y = INVALID_FLOAT_VALUE;
                        }

                        note->CalculatePositionAsPaged(displayConstants, staff->lines);

                        if (note->type == NoteType::Standard && !note->isRest)
                            note->position.y = (displayConstants.lineSpacing * measure->CalculateNoteYPositionRelativeToMeasure(noteIndex));

                        for (auto& lyric : note->lyrics)
                        {
                            lyric.CalculatePositionAsPaged(displayConstants);
                        }

                        noteIndex++;
                    }

                    for (auto noteChord : measure->noteChords)
                    {
                        noteChord->CalculatePositionAsPaged(displayConstants, staff->lines, measure, (softwareName == "MuseScore" && softwareMajorVersion == 4));
                    }

                    measureIndex++;
                }
            }
        }

        // calculate rest y positions
        for (auto instrument : instruments)
        {
            for (auto staff : instrument->staves)
            {
                for (auto measure : staff->measures)
                {
                    for (auto restNote : measure->notes)
                    {
                        if (restNote->isRest)
                        {
                            for (auto note : measure->notes)
                            {
                                if (restNote->beatPosition == note->beatPosition && restNote != note)
                                {
                                    restNote->position.y = note->position.y;

                                    float offset = 35.0f;

                                    if (note->noteStem->stemType == NoteStem::StemType::Up)
                                    {
                                        restNote->position.y += offset;
                                        break;
                                    }
                                    else if (note->noteStem->stemType == NoteStem::StemType::Down)
                                    {
                                        restNote->position.y -= offset;
                                        break;
                                    }
                                    else
                                        LOGW("this stem type is not handled");
                                }
                            }
                        }
                    }
                }
            }
        }

        // calculate width for multi measure rests
        for (auto instrument : instruments)
        {
            for (auto staff : instrument->staves)
            {
                std::vector<std::shared_ptr<Measure>> multiMeasureRests;
                int multiMeasureRestCount = 0;
                float totalMeasureWidth = 0.0f;

                std::shared_ptr<Measure> previousMultiMeasureRest = nullptr;
                std::shared_ptr<Measure> previousMeasure = nullptr;

                int measureIndex = 0;
                int systemIndex = 0;
                for (auto measure : staff->measures)
                {
                    if ((measure->startNewSystem && measureIndex != 0) || measureIndex == staff->measures.size() - 1)
                    {
                        if (measureIndex == staff->measures.size() - 1)
                        {
                            if (measure->isPartOfMultiMeasureRest)
                            {
                                measure->measureWidth = 0.0f;
                                m_MeasureWidths[measure->index] = measure->measureWidth;
                            }

                            totalMeasureWidth += measure->measureWidth;
                        }
                        //if (measure->startsMultiMeasureRest && measureIndex == staff->measures.size() - 1)
                        //{
                        //    multiMeasureRests.push_back(measure);
                        //    multiMeasureRestCount++;
                        //}

                        float systemWidth = displayConstants.pageWidth - systems[systemIndex].layout.systemLeftMargin - systems[systemIndex].layout.systemRightMargin - displayConstants.leftMargin - displayConstants.rightMargin;

                        if (previousMeasure)
                        {
                            if (previousMeasure->isPartOfMultiMeasureRest)
                            {
                                if (systemIndex + 1 < systems.size())
                                    systems[systemIndex + 1].layout.systemDistance /= 4.0f;
                            }
                        }

                        float multiMeasureRestWidth = 0.0f;
                        if (multiMeasureRestCount != 0)
                        {
                            float remainingWidth = (systemWidth - totalMeasureWidth);
                            multiMeasureRestWidth = remainingWidth / (float) multiMeasureRestCount;
                        }

                        LOGV("systemLeftMargin: %f, systemRightMargin: %f, systemWidth: %f, remainingWidth: %f, multiMeasureRestWidth: %f", systems[systemIndex].layout.systemLeftMargin, systems[systemIndex].layout.systemRightMargin,
                             systemWidth, (systemWidth - totalMeasureWidth), multiMeasureRestWidth);

                        for (auto multiMeasureRest : multiMeasureRests)
                        {
                            multiMeasureRest->measureWidth = multiMeasureRestWidth;

                            m_MeasureWidths[multiMeasureRest->index] = multiMeasureRest->measureWidth;

                            if (multiMeasureRest->startsMultiMeasureRest && multiMeasureRest->multiMeasureRestSymbol != nullptr)
                            {
                                float lineSpacing;
                                if (multiMeasureRest->type == Measure::MeasureType::Tab)
                                    lineSpacing = displayConstants.tabLineSpacing;
                                else
                                    lineSpacing = displayConstants.lineSpacing;

                                auto measureDataItem = systems[systemIndex].systemMeasureData.find(multiMeasureRest->index);

                                multiMeasureRest->multiMeasureRestSymbol->CalculateAsPaged(displayConstants, lineSpacing, staff->lines, multiMeasureRest->measureWidth, measureDataItem->second.measureBeginningWidth);
                            }
                        }

                        multiMeasureRests.clear();
                        multiMeasureRestCount = 0;
                        totalMeasureWidth = 0.0f;
                        systemIndex++;
                    }

                    if (measure->startsMultiMeasureRest)
                    {
                        multiMeasureRests.push_back(measure);
                        previousMultiMeasureRest = measure;
                        multiMeasureRestCount++;
                    }
                    else
                    {
                        /*if (previousMultiMeasureRest)
                        {
                            if (measure->index - previousMultiMeasureRest->index < previousMultiMeasureRest->numberOfMeasuresInMultiMeasureRest) // is part of multi measure rest
                            {

                            }
                        }*/

                        if (measure->isPartOfMultiMeasureRest)
                        {
                            measure->measureWidth = 0.0f;
                            m_MeasureWidths[measure->index] = measure->measureWidth;
                        }

                        totalMeasureWidth += measure->measureWidth;
                    }

                    previousMeasure = measure;
                    measureIndex++;
                }
            }
        }

        // sort all the notes by their beat positions and put them in an array of time space points
        m_TimeSpacePoints.clear();
        for (auto instrument : instruments) {
            for (auto staff : instrument->staves) {
                int measureIndex = 0;
                for (auto measure : staff->measures) {
                    TimeSpacePoint beginMeasurePoint = TimeSpacePoint();
                    beginMeasurePoint.beatPositionInSong = measure->beatPosition;
                    beginMeasurePoint.position = 0.0f;
                    beginMeasurePoint.measureIndex = measureIndex;
                    TimeSpacePoint endMeasurePoint = TimeSpacePoint();
                    endMeasurePoint.beatPositionInSong = measure->beatPosition + measure->duration.duration;
                    endMeasurePoint.position = measure->measureWidth;
                    endMeasurePoint.measureIndex = measureIndex;

                    //AddTimeSpacePoint(beginMeasurePoint);
                    //AddTimeSpacePoint(endMeasurePoint);

                    for (std::shared_ptr<Note> note : measure->notes) {

                        //if (note->isRest)
                        //    continue;

                        int insertIndex = 0;
                        bool added = false;
                        for (TimeSpacePoint point : m_TimeSpacePoints) {
                            if (note->beatPositionInSong < point.beatPositionInSong) // note should come before this point
                            {
                                TimeSpacePoint newPoint = TimeSpacePoint();
                                newPoint.beatPositionInSong = note->beatPositionInSong;
                                newPoint.position = note->position.x;
                                newPoint.measureIndex = measureIndex;
                                m_TimeSpacePoints.insert(m_TimeSpacePoints.begin() + insertIndex, newPoint); // insert note at insertIndex
                                added = true;
                                break;
                            }
                            else if (note->beatPositionInSong == point.beatPositionInSong) // note is not needed to make a new point
                            {
                                added = true;
                                break;
                            } // else note should come after orderedNote

                            insertIndex++;
                        }

                        if (!added)
                        {
                            TimeSpacePoint newPoint = TimeSpacePoint();
                            newPoint.beatPositionInSong = note->beatPositionInSong;
                            newPoint.position = note->position.x;
                            newPoint.measureIndex = measureIndex;
                            m_TimeSpacePoints.push_back(newPoint);
                        }
                    }

                    measureIndex++;
                }
            }
        }

        // calculate positions for everything else
        for (auto instrument : instruments)
        {
            for (auto staff : instrument->staves)
            {
                int measureIndex = 0;
                for (auto measure : staff->measures)
                {
                    for (auto& direction : measure->directions)
                    {
                        float defaultX = GetPositionXInMeasure(direction.beatPositionInSong, measureIndex);
                        //LOGW("PositionX of direction: Beatposition: %f, MI: %i, X: %f", direction.beatPositionInSong, measureIndex, defaultX);
                        float defaultY = -30.0f;

                        for (auto& word : direction.words)
                        {
                            word.CalculatePositionAsPaged(displayConstants, defaultX, defaultY);
                        }

                        for (auto& rehearsals : direction.rehearsals)
                        {
                            rehearsals.CalculatePositionAsPaged(displayConstants, defaultX, defaultY);
                        }

                        if (direction.metronomeMark != nullptr)
                        {
                            direction.metronomeMark->CalculatePositionAsPaged(displayConstants, defaultX, defaultY);
                        }

                        for (auto& dynamic : direction.dynamics)
                        {
                            dynamic.CalculatePositionAsPaged(displayConstants, { defaultX, defaultY });
                        }

                        if (direction.dynamicWedge != nullptr)
                        {
                            float defaultStartX = GetPositionXInMeasure(direction.dynamicWedge->beatPositionInSongStart, measureIndex);
                            float defaultStartY = -30.0f;

                            float defaultEndX = GetPositionXInMeasure(direction.dynamicWedge->beatPositionInSongEnd, measureIndex);
                            float defaultEndY = -30.0f;

                            float defaultSpread = 15.0f;

                            direction.dynamicWedge->CalculatePositionAsPaged(displayConstants, { defaultStartX, defaultStartY }, { defaultEndX, defaultEndY }, defaultSpread);
                        }

                        if (direction.bracketDirection != nullptr)
                        {
                            float defaultStartX = GetPositionXInMeasure(direction.bracketDirection->beatPositionInSongStart, measureIndex);
                            float defaultStartY = -30.0f;

                            float defaultEndX = GetPositionXInMeasure(direction.bracketDirection->beatPositionInSongEnd, measureIndex);
                            float defaultEndY = -30.0f;

                            direction.bracketDirection->CalculatePositionAsPaged(displayConstants, { defaultStartX, defaultStartY }, { defaultEndX, defaultEndY });
                        }

                        if (direction.marker != nullptr)
                        {
                            direction.marker->CalculatePositionAsPaged(displayConstants, { 0.0f, -20.0f });
                        }
                    }

                    for (auto& chord : measure->chords)
                    {
                        float defaultX = GetPositionXInMeasure(chord.beatPositionInSong, measureIndex);
                        float defaultY = -40.0f;

                        chord.CalculatePositionAsPaged(displayConstants, defaultX, defaultY);
                    }

                    for (auto tuplet : measure->tuplets)
                    {
                        tuplet->CalculatePositionAsPaged(displayConstants, { 0.0f, 0.0f }, { 0.0f, 0.0f });
                    }

                    for (auto note : measure->notes)
                    {
                        if (note->isRest && (softwareName == "MuseScore" && softwareMajorVersion == 3))
                        {
                            note->position.x = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            if (note->isFullMeasureRest)
                            {
                                note->position.x = measure->measureWidth / 2.0f;
                            }
                        }

                        if (note->tie)
                        {
                            if (note->tie->notes.second == note) // the second note of tie
                            {
                                if (measure->startNewSystem && note->tie->notes.first->measureIndex != note->tie->notes.second->measureIndex) // the notes of the tie are on different systems
                                {
                                    note->tie->isBroken = true;
                                }
                            }
                        }

                        for (auto glissSlide : note->glissSlides)
                        {
                            if (glissSlide->notes.second == note) // the second note
                            {
                                if (measure->startNewSystem && glissSlide->notes.first->measureIndex != glissSlide->notes.second->measureIndex) // the notes are on different systems
                                {
                                    glissSlide->isBroken = true;
                                }
                            }
                        }
                    }

                    for (auto noteChord : measure->noteChords)
                    {
                        for (auto note : noteChord->notes)
                        {
                            if (note->tie)
                            {
                                if (note->tie->notes.second == note) // the second note of tie
                                {
                                    if (measure->startNewSystem && note->tie->notes.first->measureIndex != note->tie->notes.second->measureIndex) // the notes of the tie are on different systems
                                    {
                                        note->tie->isBroken = true;
                                    }
                                }
                            }

                            for (auto glissSlide : note->glissSlides)
                            {
                                if (glissSlide->notes.second == note) // the second note
                                {
                                    if (measure->startNewSystem && glissSlide->notes.first->measureIndex != glissSlide->notes.second->measureIndex) // the notes are on different systems
                                    {
                                        glissSlide->isBroken = true;
                                    }
                                }
                            }
                        }
                    }


                    // calculate beam positions
                    for (BeamGroup& beamGroup : measure->beams)
                    {
                        if (!beamGroup.notes.empty())
                        {
                            std::shared_ptr<Note> firstNote = beamGroup.notes[0];
                            std::shared_ptr<Note> lastNote = beamGroup.notes[beamGroup.notes.size() - 1];

                            beamGroup.beamStartPosition.x = firstNote->position.x + firstNote->noteStem->stemPositionX;
                            beamGroup.beamStartPosition.y = firstNote->position.y + firstNote->noteStem->stemEndY;

                            beamGroup.beamEndPosition.x = lastNote->position.x + lastNote->noteStem->stemPositionX;
                            beamGroup.beamEndPosition.y = lastNote->position.y + lastNote->noteStem->stemEndY;

                            float slope = std::abs(beamGroup.GetSlope());

                            if (slope > displayConstants.maxBeamSlope) // slope needs to be smaller
                            {
                                float run = beamGroup.beamEndPosition.x - beamGroup.beamStartPosition.x;
                                float maxRise = run * displayConstants.maxBeamSlope; // since: (run * maxslope) / run = maxslope

                                if (beamGroup.beamStartPosition.y > beamGroup.beamEndPosition.y)
                                {
                                    beamGroup.beamStartPosition.y = beamGroup.beamEndPosition.y + maxRise;
                                }
                                else if (beamGroup.beamEndPosition.y > beamGroup.beamStartPosition.y)
                                {
                                    beamGroup.beamEndPosition.y = beamGroup.beamStartPosition.y + maxRise;
                                }
                            }
                        }

                        for (Beam& beam : beamGroup.beams)
                        {
                            if (!beam.notes.empty())
                            {
                                std::shared_ptr<Note> firstNote = beam.notes[0];
                                std::shared_ptr<Note> lastNote = beam.notes[beam.notes.size() - 1];

                                beam.beamStartPosition.x = firstNote->position.x + firstNote->noteStem->stemPositionX;
                                beam.beamStartPosition.y = beamGroup.GetPositionYOnBeam(beam.beamStartPosition.x);

                                beam.beamEndPosition.x = lastNote->position.x + lastNote->noteStem->stemPositionX;
                                beam.beamEndPosition.y = beamGroup.GetPositionYOnBeam(beam.beamEndPosition.x);
                            }
                            else
                                LOGE("Beam has no notes associated with it");
                        }

                        // calculate stem lengths
                        float smallestStemHeight = INFINITY;
                        float direction = 1.0f;
                        if (beamGroup.notes[0]->noteStem->stemEndY < beamGroup.notes[0]->noteStem->stemStartY)
                            direction = -1.0f;

                        for (std::shared_ptr<Note> beamNote : beamGroup.notes)
                        {
                            float beamPositionYAtNote = beamGroup.GetPositionYOnBeam(beamNote->position.x + beamNote->noteStem->stemPositionX);
                            beamNote->noteStem->stemEndY = beamPositionYAtNote - beamNote->position.y;

                            float stemHeight = beamNote->noteStem->stemEndY - beamNote->noteStem->stemStartY;

                            if (std::abs(stemHeight) < smallestStemHeight)
                            {
                                smallestStemHeight = std::abs(stemHeight);
                            }
                        }

                        if (smallestStemHeight < displayConstants.minNoteStemHeight) // stems need to be longer
                        {
                            float heightOffset = displayConstants.minNoteStemHeight - smallestStemHeight; // how much to offset the beam
                            heightOffset *= direction;
                            heightOffset += 20.0f * direction; // TODO: temp

                            beamGroup.beamStartPosition.y += heightOffset;
                            beamGroup.beamEndPosition.y += heightOffset;

                            for (Beam& beam : beamGroup.beams)
                            {
                                beam.beamStartPosition.y += heightOffset;
                                beam.beamEndPosition.y += heightOffset;
                            }

                            // recalculate stem lengths
                            for (std::shared_ptr<Note> beamNote : beamGroup.notes)
                            {
                                float beamPositionYAtNote = beamGroup.GetPositionYOnBeam(beamNote->position.x + beamNote->noteStem->stemPositionX);
                                beamNote->noteStem->stemEndY = beamPositionYAtNote - beamNote->position.y;
                            }
                        }
                    }

                    measureIndex++;
                }
            }
        }
    }
    /*else if (settings.musicLayout == Settings::MusicLayout::Vertical || settings.musicLayout == Settings::MusicLayout::Horizontal)
    {
        // sort all the notes by their stop times(beatPositionInSong + duration) from smallest to largest
        std::vector<std::shared_ptr<Note>> orderedNotes;
        for (auto instrument : instruments) {
            for (auto staff : instrument->staves) {
                int measureIndex = 0;
                for (auto measure : staff->measures) {
                    float measureBeatPosition = staff->GetMeasureBeatPosition(measureIndex);

                    for (std::shared_ptr<Note> note : measure->notes) {
                        float noteBeatPosition = note->beatPosition + measureBeatPosition;
                        float noteBeatLength = note->duration.duration;

                        int insertIndex = 0;
                        for (std::shared_ptr<Note> orderedNote : orderedNotes) {
                            if (orderedNote->beatPositionInSong + orderedNote->duration.duration > note->beatPositionInSong + note->duration.duration)
                            {
                                break; // note should come before orderedNote
                            } // else note should come after orderedNote

                            insertIndex++;
                        }
                        orderedNotes.insert(orderedNotes.begin() + insertIndex, note); // insert note at insertIndex
                    }

                    measureIndex++;
                }
            }
        }

        std::vector<float> measuresNotesWidths; // the widths of the combined widths of all the notes in each measure

        m_MinNoteData.clear();
        m_MeasureWidths.clear();
        m_MeasureBeginWidths.clear();
        // calculate m_MinNoteData
        {
            float time = 0.0f; // current time in song
            int steps = 0; // debug var
            int measureIndex = 0;
            float width = 0.0f;
            int noteIndex = 0;
            for (std::shared_ptr<Note> note : orderedNotes) {
                //LOGD("stop position %f", note->beatPositionInSong + note->duration.duration);
                float startTime = note->beatPositionInSong;
                if (startTime >= time) {
                    time += note->duration.duration;
                    steps++;

                    //LOGD("note %i: beatpos: %f, duration: %f, width: %f, total width: %f", noteIndex, note->beatPositionInSong, note->duration.duration, note->GetMinWidth(), width);
                    //std::array<float, 3> a = { note->GetMinWidth(), note->beatPositionInSong, note->duration };
                    //m_MinNoteData.push_back({ note->GetMinWidth(), note->beatPositionInSong, note->duration.duration });
                    m_MinNoteData.emplace_back(GetNoteMinWidthInFront(note), note->beatPositionInSong, note->duration.duration, note->measureIndex );

                    if (measureIndex != note->measureIndex) {
                        measureIndex = note->measureIndex;
                        //measuresNotesWidths.push_back(width);
                        width = 0.0f;
                    }

                    width += GetNoteMinWidthInFront(note); // moved
                }

                if (noteIndex >= orderedNotes.size()-1) // is last note
                {
                    //measuresNotesWidths.push_back(width);
                }

                noteIndex++;
            }
            LOGD("time: %f", time);
            LOGD("steps: %i", steps);
        }

        {
            int nx_debug = 0;
            for (auto note : orderedNotes) {
                float noteStartX = -1.0f;
                float noteStopX = -1.0f;
                float position = 0.0f;
                float time = 0.0f;

                NoteData* previousNoteData = nullptr;

                std::vector<NoteData*> noteDatas;
                for (auto& minNoteData: m_MinNoteData) {
                    if (minNoteData.beatPositionInSong == note->beatPositionInSong && minNoteData.duration == note->duration.duration) // the same note
                        break;

                    // changed minNoteData.position to position in the if/else statement (this comment is only here be cause I could have caused a bug)
                    if (position >= note->beatPositionInSong + note->duration.duration) // noteData.beatPositionInSong is past(or equal to) note stop beat position
                    {
                        if (previousNoteData != nullptr)
                        {
                            float stopBeatPosition = note->beatPositionInSong + note->duration.duration;
                            // calculates how much of the previous note's width should be taken away from the position
                            float diff = (stopBeatPosition - previousNoteData->beatPositionInSong); // num of beats between beatPositionInSong and previousNoteBeatPosition
                            float howMuch = (diff/previousNoteData->duration); // the percentage of previousNoteWidth that needs to be added to previousNoteX to get the x of beatPositionInSong
                            float howMuchToSub = 1.0f - howMuch; // the percentage of previousNoteWidth that needs to be subtracted from currentNoteX to get the x of beatPositionInSong
                            noteStopX = position - (previousNoteData->width * howMuchToSub); // get the percentage of previousNoteWidth and subtract
                            break;
                        }
                    }
                    else if (position >= note->beatPositionInSong) // noteData.beatPositionInSong is past(or equal to) note->beatPositionInSong(what we want the x position of)
                    {
                        noteDatas.push_back(&minNoteData);

                        if (noteStartX == -1.0f) // if noteStartX is not set yet
                        {
                            if (minNoteData.beatPositionInSong != note->beatPositionInSong) // add previousNoteData
                                noteDatas.push_back(previousNoteData);

                            if (previousNoteData == nullptr) // noteData is the first in the list so the start position is just 0.0f
                            {
                                noteStartX = 0.0f;
                            }
                            else
                            {
                                // calculates how much of the previous note's width should be taken away from the position
                                float diff = (note->beatPositionInSong - previousNoteData->beatPositionInSong); // num of beats between beatPositionInSong and previousNoteBeatPosition
                                float howMuch = (diff/previousNoteData->duration); // the percentage of previousNoteWidth that needs to be added to previousNoteX to get the x of beatPositionInSong
                                float howMuchToSub = 1.0f - howMuch; // the percentage of previousNoteWidth that needs to be subtracted from currentNoteX to get the x of beatPositionInSong
                                noteStartX = position - (previousNoteData->width * howMuchToSub); // get the percentage of previousNoteWidth and subtract
                            }
                        }
                    }

                    position += minNoteData.width;
                    time += minNoteData.duration;
                    previousNoteData = &minNoteData;
                    //previousNoteData.beatPositionInSong = minNoteData.beatPositionInSong;
                    //previousNoteData.duration = minNoteData.duration;
                    //previousNoteData.width = minNoteData.width;
                    //if (note->beatPositionInSong >= minNoteData.beatPositionInSong && note->beatPositionInSong <= minNoteData.beatPositionInSong + minNoteData.duration) {
                    //    float pos = note->beatPositionInSong
                    //}
                    //position
                } // m_MinNoteData for loop

                float noteWidth = noteStopX - noteStartX;
                float minNoteWidth = GetNoteMinWidthInFront(note);
                //LOGD("noteWidth: %f, minNoteWidth: %f, noteStopX: %f, noteStartX: %f", noteWidth, minNoteWidth, noteStopX, noteStartX);
                if (noteWidth < minNoteWidth) // then fix it so that noteWidth >= minNoteWidth
                {
                    //LOGD("noteWidth %i < minNoteWidth %i", nx_debug, noteDatas.size());
                    float howMuchToAdd = minNoteWidth - noteWidth;
                    for (auto* noteData: noteDatas) {
                        float w = 0.0f;
                        if (note->beatPositionInSong > noteData->beatPositionInSong)
                        {
                            w = (noteData->beatPositionInSong + noteData->duration) - note->beatPositionInSong;
                        }
                        else if (note->beatPositionInSong + note->duration.duration >= noteData->beatPositionInSong && note->beatPositionInSong + note->duration.duration < noteData->beatPositionInSong + noteData->duration)
                        {
                            w = (note->beatPositionInSong + note->duration.duration) - noteData->beatPositionInSong;
                        }
                        else
                        {
                            w = noteData->duration;
                        }

                        float pw = w / note->duration.duration;
                        noteData->width += pw * howMuchToAdd;
                        //LOGD("pw = %f; w = %f; howMuchToAdd = %f; pw * howMuchToAdd = %f", pw, w, howMuchToAdd, pw * howMuchToAdd);
                    }
                } // noteWidth < minNoteWidth if statement
                nx_debug++;
            } // ordered notes for loop
        }

        // calculate m_MinNoteData
        {
            float time = 0.0f; // current time in song
            int steps = 0; // debug var
            int measureIndex = 0;
            float width = 0.0f;
            int noteIndex = 0;
            for (auto noteData : m_MinNoteData) {

                if (measureIndex != noteData.measureIndex) {
                    //LOGV("----: M_I: %i, N_M_I: %i, W: %f, N_I: %i", measureIndex, noteData.measureIndex, width, noteIndex);
                    measureIndex = noteData.measureIndex;
                    measuresNotesWidths.push_back(width);
                    width = 0.0f;
                }

                if (noteIndex >= m_MinNoteData.size()-1) // is last note
                {
                    measuresNotesWidths.push_back(width);
                }

                width += noteData.width;
                noteIndex++; // added this (this comment is only here be cause I could have caused a bug)
            }
        }

        // calculate measure widths
        for (auto instrument : instruments) {
            for (auto staff : instrument->staves) {
                int measureIndex = 0;
                for (auto measure : staff->measures) {
                    // these were ints (this comment is only here be cause I could have caused a bug)
                    float minWidth = measure->CalculateMinWidth(measuresNotesWidths[measureIndex]);
                    //float beginWidth = measure->GetBeginningWidth();
                    float beginWidth = 20.0f;

                    if ((int)m_MeasureWidths.size() - 1 >= measureIndex) { // if there is all ready a width at measureNumber
                        if (m_MeasureWidths[measureIndex] < minWidth) { // if the width of this measure is bigger than the widths of the previous measures
                            m_MeasureWidths[measureIndex] = minWidth;
                        }

                        if (m_MeasureBeginWidths[measureIndex] < beginWidth) { // if the begin width of this measure is bigger than the begin widths of the previous measures
                            m_MeasureBeginWidths[measureIndex] = beginWidth;
                        }
                    } else { // else push back width
                        m_MeasureWidths.push_back(minWidth);
                        m_MeasureBeginWidths.push_back(beginWidth);
                    }

                    measureIndex++;
                }
            }
        }

        for (int i = 0; i < m_MeasureWidths.size(); i++)
        {
            //LOGD("width: %f pos: %f notesWidth: %f", m_MeasureWidths[i], GetMeasurePositionX(i), measuresNotesWidths[i]);
        }

        for (auto instrument : instruments)
        {
            for (auto staff : instrument->staves)
            {
                int measureIndex = 0;
                for (auto measure : staff->measures)
                {
                    measure->measureWidth = GetMeasureWidth(measureIndex);

                    int noteIndex = 0;
                    for (auto note : measure->notes)
                    {
                        if (note->isRest) { // is a rest
                            float ls;
                            if (note->type == NoteType::Tab)
                                ls = displayConstants.tabLineSpacing;
                            else
                                ls = displayConstants.lineSpacing;
                            note->position.x = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->position.y = ((ls * float(staff->lines - 1)) / 2.0f);
                        }
                        else if (note->type == NoteType::Tab) // is a tab note
                        {
                            note->position.x = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->position.y = (displayConstants.tabLineSpacing * float(note->string - 1));
                        }
                        else // is a standard note
                        {
                            note->position.x = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->position.y = (displayConstants.lineSpacing * measure->CalculateNoteYPositionRelativeToMeasure(noteIndex));
                        }

                        noteIndex++;
                    }

                    measureIndex++;
                }
            }
        }
    }*/

    int numPages = GetNumPages();

    Vec2<float> pageNumberDefaultPosition = { displayConstants.pageWidth / 2.0f, displayConstants.pageHeight * (17.0f / 18.0f) };
    for (int i = 0; i < numPages; i++)
    {
        PageNumber newPageNumber(i + 1);

        newPageNumber.CalculatePosition(displayConstants, pageNumberDefaultPosition);

        pageNumbers.push_back(newPageNumber);
    }

    LOGD("done updating song data");
}

void Song::AddTimeSpacePoint(TimeSpacePoint point)
{
    bool added = false;
    int insertIndex = 0;
    for (TimeSpacePoint compare : m_TimeSpacePoints) {
        if (point.beatPositionInSong < compare.beatPositionInSong) // point should come before compare
        {
            m_TimeSpacePoints.insert(m_TimeSpacePoints.begin() + insertIndex, point); // insert point at insertIndex
            added = true;
            break;
        }
        else if (point.beatPositionInSong == compare.beatPositionInSong) // point is not needed
        {
            added = true;
            break;
        } // else point should come after compare

        insertIndex++;
    }

    if (!added)
        m_TimeSpacePoints.push_back(point);
}

void Song::CalculateNoteBeatPositionsInSong()
{
    for (auto instrument : instruments) {
        for (auto staff : instrument->staves) {
            int measureIndex = 0;
            for (auto measure : staff->measures) {
                float measureBeatPosition = staff->GetMeasureBeatPosition(measureIndex);

                for (std::shared_ptr<Note> note : measure->notes) {
                    note->beatPositionInSong = note->beatPosition + measureBeatPosition;
                }

                for (Chord& chord : measure->chords) {
                    chord.beatPositionInSong = chord.beatPosition + measureBeatPosition;
                }

                for (Direction& direction : measure->directions) {
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

                measureIndex++;
            }
        }
    }
}

float Song::GetNoteMinWidthInFront(std::shared_ptr<Note> note) const
{
    return note->GetMinWidth();
}

float Song::GetNoteMinWidthBehind(std::shared_ptr<Note> note) const
{
    return 0.0f;
}

float Song::GetMeasureWidth(int measureIndex) const
{
    if (m_MeasureWidths.size() > measureIndex) {
        return m_MeasureWidths[measureIndex];
    }
    return 0.0f;
}

float Song::GetMeasurePositionX(int measureIndex) const
{
    float position = 0.0f;
    if (DoesMeasureStartNewSystem(measureIndex))
        return position;
    else
    {
        int systemIndex = GetSystemIndex(measureIndex);
        int firstMeasureInSystemIndex = GetFirstMeasureInSystem(systemIndex);

        for (int i = firstMeasureInSystemIndex; i < measureIndex; i++)
        {
            position += GetMeasureWidth(i);
        }

        return position;
    }
}

float Song::GetMeasureBeatPosition(int measureIndex) const
{
    float beatPosition = 0.0f;
    if (!instruments.empty())
    {
        if (!instruments[0]->staves.empty())
        {
            beatPosition = instruments[0]->staves[0]->GetMeasureBeatPosition(measureIndex);
        }
    }
    return beatPosition;
}

float Song::GetMeasurePositionY(int measureIndex) const
{
    return 0.0f;
}

float Song::GetSongWidth() const
{
    float songWidth = 0.0f;
    for (float w : m_MeasureWidths)
    {
        songWidth += w;
    }
    return songWidth;
}

float Song::GetPositionXInMeasure(float beatPositionInSong, int currentMeasureIndex) const
{
    float position = 0.0f;

    if (settings.musicLayout == Settings::MusicLayout::Paged)
    {
        //LOGD("GetPositionXInMeasure! %f", beatPositionInSong);
        TimeSpacePoint previousPoint = TimeSpacePoint();
        for (const TimeSpacePoint& point : m_TimeSpacePoints)
        {
            if (currentMeasureIndex == point.measureIndex)
            {
                if (FloatsAreEqual(beatPositionInSong, point.beatPositionInSong))
                {
                    position = point.position;
                    //LOGE("beatPositionInSong == point.beatPosition: %f", position);
                    //LOGE("beatPositionInSong == point.beatPosition: bpos: %f, pos: %f, pointBP: %f, prePointBP: %f, pMI: %d, prePMI: %d, pointP: %f, prePointP: %f", beatPositionInSong, position, point.beatPosition, previousPoint.beatPosition, point.measureIndex, previousPoint.measureIndex, point.position, previousPoint.position);

                    break;
                }
                else if (beatPositionInSong < point.beatPositionInSong)
                {
                    float previousBeatPosition = previousPoint.beatPositionInSong;
                    float previousPosition = previousPoint.position;
                    if (currentMeasureIndex != previousPoint.measureIndex)
                    {
                        previousBeatPosition = GetMeasureBeatPosition(currentMeasureIndex);
                        previousPosition = 0.0f;
                    }

                    float posRange = (point.beatPositionInSong - previousBeatPosition);
                    if (posRange == 0)
                        LOGE("divide by 0!!!!!!!!!!");
                    float percentage = (beatPositionInSong - previousBeatPosition) / posRange;
                    position = (point.position - previousPosition) * percentage;
                    position += previousPosition;

                    //LOGE("beatPositionInSong < point.beatPosition: bpos: %f, pos: %f, posRange: %f, percentage: %f, pointBP: %f, prePointBP: %f, pMI: %d, prePMI: %d, pointP: %f, prePointP: %f", beatPositionInSong, position, posRange, percentage, point.beatPosition, previousPoint.beatPosition, point.measureIndex, previousPoint.measureIndex, point.position, previousPoint.position);

                    break;
                }

                /*if (beatPositionInSong < point.beatPosition)
                {
                    float posRange = (point.beatPosition - previousPoint.beatPosition);
                    if (posRange == 0)
                        LOGE("divide by 0!!!!!!!!!!");
                    float percentage = (beatPositionInSong - previousPoint.beatPosition) / posRange;
                    position = (point.position - previousPoint.position) * percentage;

                    LOGE("beatPositionInSong < point.beatPosition: bpos: %f, pos: %f, posRange: %f, percentage: %f, pointBP: %f, prePointBP: %f, pointP: %f, prePointP: %f", beatPositionInSong, position, posRange, percentage, point.beatPosition, previousPoint.beatPosition, point.position, previousPoint.position);

                    break;
                }
                else if (beatPositionInSong == point.beatPosition) // TODO: fix dangerous comparision
                {
                    position = point.position;
                    LOGE("beatPositionInSong == point.beatPosition: %f", position);
                    break;
                }*/
            }

            previousPoint = point;
        }
    }
    else
    {
        position += m_MeasureBeginWidths[currentMeasureIndex];
        // add the measure's beginning width <--

        /*float measureBeatPosition = 0.0f;
        if (!instruments.empty())
        {
            if (!instruments[0]->staves.empty())
            {
                if (!instruments[0]->staves[0]->measures.empty())
                {
                    measureBeatPosition = instruments[0]->staves[0]->measures[0]->beatPosition;
                }
            }
        }*/

        float pos = 0.0f; // debug var
        float time = 0.0f;

        float previousNoteWidth = 0.0f;
        float previousNoteBeatPosition = 0.0f;
        float previousNoteDuration = 0.0f;
        for (auto noteData: m_MinNoteData)
        {
            if (time >= GetMeasureBeatPosition(currentMeasureIndex)) // should be equal first time that it is true
            {
                if (previousNoteBeatPosition == 0.0f) // first time, set to the start of the measure
                {
                    previousNoteBeatPosition = GetMeasureBeatPosition(currentMeasureIndex);
                }

                if (noteData.beatPositionInSong >= beatPositionInSong) // noteData.beatPositionInSong is past(or equal to) beatPositionInSong(what we want the x position of)
                {
                    if (previousNoteDuration == 0.0f) // noteData is the first in the list so the position is just 0.0f
                    {
                        break; // done
                    }

                    // calculates how much of the previous note's width should be taken away from the position
                    float diff = (beatPositionInSong - previousNoteBeatPosition); // num of beats between beatPositionInSong and previousNoteBeatPosition
                    float howMuch = (diff / previousNoteDuration); // the percentage of previousNoteWidth that needs to be added to previousNoteX to get the x of beatPositionInSong
                    float howMuchToSub = 1.0f - howMuch; // the percentage of previousNoteWidth that needs to be subtracted from currentNoteX to get the x of beatPositionInSong
                    position -= (previousNoteWidth * howMuchToSub); // get the percentage of previousNoteWidth and subtract
                    break; // done
                }

                position += noteData.width;
                previousNoteBeatPosition = noteData.beatPositionInSong;
                previousNoteDuration = noteData.duration;
            }
            pos += noteData.width;
            time += noteData.duration;
            previousNoteWidth = noteData.width;
        }
    }

    return position;
}

float Song::GetPositionXInSong(float beatPositionInSong, int currentMeasureIndex) const
{
    float position = GetPositionXInMeasure(beatPositionInSong, currentMeasureIndex);

    float measurePositionX = GetMeasurePositionX(currentMeasureIndex);
    position += measurePositionX;

    return position;
}

std::shared_ptr<Measure> Song::GetMeasure(int measureIndex) const
{
    if (!instruments.empty())
        if (!instruments[0]->staves.empty())
            if (instruments[0]->staves[0]->measures.size() > measureIndex)
                return instruments[0]->staves[0]->measures[measureIndex];
    return nullptr;
}

bool Song::DoesMeasureStartNewSystem(int measureIndex) const
{
    std::shared_ptr<Measure> measure = GetMeasure(measureIndex);
    return measure->startNewSystem;
}

bool Song::DoesMeasureStartNewPage(int measureIndex) const
{
    std::shared_ptr<Measure> measure = GetMeasure(measureIndex);

    if (measure != nullptr)
        return measure->startNewPage;
    else
        throw DoesNotExistException("Measure does not exist");
}

float Song::GetSystemPositionY(int measureIndex) const // TODO: needs finnished
{
    float instYPosition = 0.0f;
    int instrumentIndex = 0;
    std::shared_ptr<Instrument> prevInstrument = nullptr;
    for (auto instrument: instruments) {

        if (songData.instrumentInfos[instrumentIndex].visible) {
            if (prevInstrument != nullptr)
                instYPosition += prevInstrument->GetMiddleHeight(10.0f, 13.33f, 0,
                                                                 instrument->GetMeasureCount()) +
                                 prevInstrument->GetBelowHeight(10.0f, 13.33f, 0,
                                                                instrument->GetMeasureCount());
            instYPosition += instrument->GetAboveHeight(10.0f, 13.33f, 0,
                                                        instrument->GetMeasureCount());

            prevInstrument = instrument;
        } // instrument is visible
        instrumentIndex++;
    } // instruments loop

    instYPosition += 100.0f;
    return instYPosition * (measureIndex/4);
}

float Song::GetSystemHeight(int measureIndex) const
{
    return 150.0f;
}

int Song::GetSystemIndex(int measureIndex) const
{
    int systemIndex = -1;

    if (instruments.empty())
        throw IsEmptyException("Instruments empty");
    if (instruments[0]->staves.empty())
        throw IsEmptyException("Staves empty");

    for (auto measure : instruments[0]->staves[0]->measures)
    {
        if (measure == nullptr)
            throw IsNullException("Measure is nullptr");

        if (measure->startNewSystem)
            systemIndex++;

        if (measure->index == measureIndex)
            break;
    }

    if (systemIndex == -1)
        throw BaseException();

    return systemIndex;
}

int Song::GetPageIndex(int measureIndex) const
{
    int pageIndex = -1;

    if (instruments.empty())
        return 0;
    if (instruments[0]->staves.empty())
        return 0;

    for (auto measure : instruments[0]->staves[0]->measures)
    {
        if (measure->startNewPage)
            pageIndex++;

        if (measure->index == measureIndex)
            break;
    }

    return pageIndex;
}

int Song::GetFirstMeasureOnPage(int pageIndex) const
{
    int measureIndex = 0;

    if (instruments.empty())
        return 0;
    if (instruments[0]->staves.empty())
        return 0;

    int i = -1;
    for (auto measure : instruments[0]->staves[0]->measures)
    {
        if (measure->startNewPage)
            i++;

        if (i == pageIndex)
            break;

        measureIndex++;
    }

    return measureIndex;
}

int Song::GetFirstMeasureInSystem(int systemIndex) const
{
    int measureIndex = 0;

    if (instruments.empty())
        return 0;
    if (instruments[0]->staves.empty())
        return 0;

    int i = -1;
    for (auto measure : instruments[0]->staves[0]->measures)
    {
        if (measure->startNewSystem)
            i++;

        if (i == systemIndex)
            break;

        measureIndex++;
    }

    return measureIndex;
}

int Song::GetNumPages() const
{
    int numPages = 0;

    if (instruments.empty())
        throw IsEmptyException();
    if (!instruments[0])
        throw IsNullException();
    if (instruments[0]->staves.empty())
        throw IsEmptyException();
    if (!instruments[0]->staves[0])
        throw IsNullException();

    for (auto measure : instruments[0]->staves[0]->measures)
    {
        if (measure)
        {
            if (measure->startNewPage)
                numPages++;
        }
    }

    return numPages;
}

/*float Song::GetMeasurePositionX(int measureIndex, Vec2<float> pagePosition, Vec2<float> systemPosition)
{
    if (DoesMeasureStartNewSystem(measureIndex)) // if it is the first measure in the system
        return pagePosition.x + systemPosition.x;

    float measurePositionX = 0.0f;

    int systemIndex = GetSystemIndex(measureIndex);
    int firstMeasureInSystemIndex = GetFirstMeasureInSystem(systemIndex);

    int nextMeasureIndex = firstMeasureInSystemIndex;
    float currentPositionX = systemPosition.x;

    while (true)
    {
        if (nextMeasureIndex == measureIndex)
        {
            break;
        }

        float width = GetMeasureWidth(nextMeasureIndex);

        currentPositionX += width;

        nextMeasureIndex++;

        if (nextMeasureIndex - firstMeasureInSystemIndex >= 100) // probably a infinite loop
        {
            LOGE("INFINITE LOOP");
            break;
        }
    }

    measurePositionX = currentPositionX;

    return measurePositionX;
}*/

float Song::GetInstrumentPositionY(int measureIndex, int instrumentIndex) const
{
    float instrumentPositionY = 0.0f;

    std::shared_ptr<Instrument> prevInstrument = nullptr;
    int currentInstrumentIndex = 0;
    for (auto instrument : instruments)
    {
        if (prevInstrument != nullptr)
        {
            int firstMeasureIndex = GetFirstMeasureInSystem(GetSystemIndex(measureIndex));
            instrumentPositionY += prevInstrument->GetMiddleHeight(10.0f, 13.33f, 0, instrument->GetMeasureCount());
            instrumentPositionY += instrument->staves.front()->measures[firstMeasureIndex]->staffDistance;
        }

        if (currentInstrumentIndex == instrumentIndex)
        {
            break;
        }

        prevInstrument = instrument;
        currentInstrumentIndex++;
    }

    return instrumentPositionY;
}

void Song::UpdateBoundingBoxes(const std::vector<Vec2<float>>& pagePositions, const std::vector<Vec2<float>>& systemPositions)
{
    if (pagePositions.empty() || systemPositions.empty())
        throw IsEmptyException("Missing positioning data");

    int instrumentIndex = 0;
    for (auto instrument : instruments) {

        int staffIndex = 0;
        for (auto staff : instrument->staves) {
            float ls = displayConstants.lineSpacing;
            if (staff->type == Staff::StaffType::Tab) {
                ls = displayConstants.tabLineSpacing;
            }

            int measureIndex = 0;
            for (auto measure : staff->measures) {

                float instPositionY = GetInstrumentPositionY(measureIndex, instrumentIndex) + systemPositions[GetSystemIndex(measureIndex)].y + pagePositions[GetPageIndex(measureIndex)].y;
                //float instPositionY = systemPositions[GetSystemIndex(measureIndex)].y;

                // staff y position
                float staffPositionY = 0.0f;
                if (staffIndex == 0) {
                    staffPositionY = 0.0f;
                } else if (staffIndex >= 1) {
                    staffPositionY += staff->measures[measureIndex]->staffDistance + instrument->staves[staffIndex - 1]->GetMiddleHeight(ls);
                }

                //LOGD("instPosY: %f, staffPosY: %f", instPositionY, staffPositionY);

                int pageIndex = GetPageIndex(measureIndex);
                int systemIndex = GetSystemIndex(measureIndex);
                if (pageIndex < pagePositions.size() && systemIndex < systemPositions.size())
                {
                    Vec2<float> pagePosition = pagePositions[pageIndex];
                    Vec2<float> systemPosition = systemPositions[systemIndex];

                    //LOGD("pagePosition: x: %f, y: %f, systemPosition: x: %f, y: %f", pagePosition.x, pagePosition.y, systemPosition.x, systemPosition.y);

                    float measurePositionX = GetMeasurePositionX(measureIndex);

                    float measureHeight;

                    if (staff->type == Staff::StaffType::Standard)
                    {
                        measureHeight = float(staff->lines - 1) * displayConstants.lineSpacing;
                    }
                    else
                    {
                        measureHeight = float(staff->lines - 1) * displayConstants.tabLineSpacing;
                    }

                    //LOGW("measurePositionX: %f", measurePositionX);
                    measure->UpdateBoundingBoxes(displayConstants, {measurePositionX + systemPosition.x + pagePosition.x, instPositionY + staffPositionY }, measureHeight); // this line crashes
                }

                measureIndex++;
            }

            staffIndex++;
        }

        instrumentIndex++;
    }
}

void Song::RenderBoundingBoxes(RenderData& renderData, const std::vector<Vec2<float>>& pagePositions, const std::vector<Vec2<float>>& systemPositions)
{
    for (auto instrument : instruments) {
        for (auto staff : instrument->staves) {
            int measureIndex = 0;
            for (auto measure : staff->measures) {

                measure->RenderDebug(renderData);

                measureIndex++;
            }
        }
    }
}

void Song::ResolveCollisions()
{
    for (auto instrument : instruments) {
        for (auto staff : instrument->staves) {

            int measureIndex = 0;
            int pageIndex = -1;
            for (auto measure : staff->measures) {

                if (DoesMeasureStartNewPage(measureIndex))
                    pageIndex++;

                ResolveCollisionsWith(measure->boundingBox, pageIndex);
                ResolveCollisionsWith(measure->measureNumber.boundingBox, pageIndex);
                ResolveCollisionsWith(measure->clef.boundingBox, pageIndex);

                for (auto note : measure->notes)
                {
                    ResolveCollisionsWith(note->boundingBox, pageIndex);

                    if (note->fermata)
                    {
                        BoundingBox oldBoundingBox = note->fermata->boundingBox;
                        ResolveCollisionsWith(note->fermata->boundingBox, pageIndex);
                        note->fermata->Move(note->fermata->boundingBox.position - oldBoundingBox.position);
                    }

                    for (auto ornament : note->ornaments)
                    {
                        if (ornament)
                        {
                            BoundingBox oldBoundingBox = ornament->boundingBox;
                            ResolveCollisionsWith(ornament->boundingBox, pageIndex);
                            ornament->Move(ornament->boundingBox.position - oldBoundingBox.position);
                        }
                    }

                    for (auto& lyric : note->lyrics)
                    {
                        ResolveCollisionsWith(lyric.boundingBox, pageIndex);
                    }
                }

                for (auto noteChord : measure->noteChords)
                {
                    ResolveCollisionsWith(noteChord->boundingBox, pageIndex);
                }

                for (auto& beamGroup : measure->beams)
                {
                    ResolveCollisionsWith(beamGroup.boundingBox, pageIndex);
                }

                for (Direction& direction : measure->directions) {

                    for (auto& words : direction.words)
                    {
                        BoundingBox oldBoundingBox = words.boundingBox;
                        ResolveCollisionsWith(words.boundingBox, pageIndex);
                        words.Move(words.boundingBox.position - oldBoundingBox.position);
                    }

                    for (auto& rehearsal : direction.rehearsals)
                    {
                        BoundingBox oldBoundingBox = rehearsal.boundingBox;
                        ResolveCollisionsWith(rehearsal.boundingBox, pageIndex);
                        rehearsal.Move(rehearsal.boundingBox.position - oldBoundingBox.position);
                    }

                    for (auto& dynamic : direction.dynamics)
                    {
                        BoundingBox oldBoundingBox = dynamic.boundingBox;
                        ResolveCollisionsWith(dynamic.boundingBox, pageIndex);
                        dynamic.Move(dynamic.boundingBox.position - oldBoundingBox.position);
                    }

                    if (direction.dynamicWedge != nullptr)
                    {
                        BoundingBox oldBoundingBox = direction.dynamicWedge->boundingBox;
                        ResolveCollisionsWith(direction.dynamicWedge->boundingBox, pageIndex);
                        direction.dynamicWedge->Move(direction.dynamicWedge->boundingBox.position - oldBoundingBox.position);
                    }

                    if (direction.bracketDirection != nullptr)
                    {
                        BoundingBox oldBoundingBox = direction.bracketDirection->boundingBox;
                        ResolveCollisionsWith(direction.bracketDirection->boundingBox, pageIndex);
                        direction.bracketDirection->Move(direction.bracketDirection->boundingBox.position - oldBoundingBox.position);
                    }

                    if (direction.metronomeMark != nullptr)
                    {
                        BoundingBox oldBoundingBox = direction.metronomeMark->boundingBox;
                        ResolveCollisionsWith(direction.metronomeMark->boundingBox, pageIndex);
                        direction.metronomeMark->Move(direction.metronomeMark->boundingBox.position - oldBoundingBox.position);
                    }

                    if (direction.marker != nullptr)
                    {
                        BoundingBox oldBoundingBox = direction.marker->boundingBox;
                        ResolveCollisionsWith(direction.marker->boundingBox, pageIndex);
                        direction.marker->Move(direction.marker->boundingBox.position - oldBoundingBox.position);
                    }
                }

                for (Chord& chord : measure->chords)
                {
                    BoundingBox oldBoundingBox = chord.boundingBox;
                    ResolveCollisionsWith(chord.boundingBox, pageIndex);
                    chord.Move(chord.boundingBox.position - oldBoundingBox.position);
                }

                measureIndex++;
            }
        }
    }
}

void Song::ResolveCollisionsWith(BoundingBox& box, int pageIndex)
{
    for (auto instrument : instruments) {
        for (auto staff : instrument->staves) {
            int measureIndex = 0;
            int currentPageIndex = -1;

            for (auto measure : staff->measures) {

                if (measure->startNewPage)
                    currentPageIndex++;

                if (currentPageIndex == pageIndex)
                {
                    for (auto note : measure->notes)
                    {
                        for (auto ornament : note->ornaments)
                        {
                            if (ornament)
                            {
                                ResolveCollisionWith(ornament, box);
                            }
                        }

                        for (auto& lyric : note->lyrics)
                        {
                            ResolveCollisionWith(&lyric, box);
                        }
                    }

                    for (Direction& direction : measure->directions)
                    {

                        for (auto& words : direction.words)
                        {
                            BoundingBox oldBoundingBox = words.boundingBox;
                            BoundingBox::ResolveOverlap(box, words.boundingBox);
                            words.Move(words.boundingBox.position - oldBoundingBox.position);
                        }

                        for (auto& rehearsal : direction.rehearsals)
                        {
                            /*LOGV("rehearsal text: %s, box: %s | rehearsal.box: %s", rehearsal.text.string.c_str(), box.GetPrintableString().c_str(), rehearsal.boundingBox.GetPrintableString().c_str());
                            if (BoundingBox::DoBoundingBoxesOverlap(box, rehearsal.boundingBox))
                            {
                                LOGW("rehearsal text: %s", rehearsal.text.string.c_str());
                            }*/
                            BoundingBox oldBoundingBox = rehearsal.boundingBox;
                            BoundingBox::ResolveOverlap(box, rehearsal.boundingBox);
                            rehearsal.Move(rehearsal.boundingBox.position - oldBoundingBox.position);
                        }

                        if (direction.metronomeMark != nullptr)
                        {
                            ResolveCollisionWith(direction.metronomeMark, box);
                            //LOGW("metrnomeMark!: %s", direction.metronomeMark->tempo.c_str());
                            //BoundingBox::ResolveOverlap(box, direction.metronomeMark->boundingBox);
                        }

                        for (auto& dynamic : direction.dynamics)
                        {
                            ResolveCollisionWith(&dynamic, box);
                        }

                        if (direction.dynamicWedge != nullptr)
                        {
                            ResolveCollisionWith(direction.dynamicWedge, box);
                        }

                        if (direction.bracketDirection != nullptr)
                        {
                            ResolveCollisionWith(direction.bracketDirection, box);
                        }

                        if (direction.marker)
                        {
                            ResolveCollisionWith(direction.marker, box);
                        }
                    }

                    for (Chord& chord : measure->chords)
                    {
                        ResolveCollisionWith(&chord, box);
                    }
                }

                measureIndex++;
            }
        }
    }
}

void Song::ResolveCollisionWith(std::shared_ptr<VisibleElement> element, BoundingBox& box)
{
    BoundingBox oldBoundingBox = element->boundingBox;
    BoundingBox::ResolveOverlap(box, element->boundingBox);
    element->Move(element->boundingBox.position - oldBoundingBox.position);
}

void Song::ResolveCollisionWith(VisibleElement* element, BoundingBox& box)
{
    BoundingBox oldBoundingBox = element->boundingBox;
    BoundingBox::ResolveOverlap(box, element->boundingBox);
    element->Move(element->boundingBox.position - oldBoundingBox.position);
}

std::vector<Vec2<float>> Song::GetSystemPositions() const
{
    std::vector<Vec2<float>> systemPositions;

    int systemIndex = 0;

    Vec2<float> systemPosition;

    float previousSystemPositionY = 0.0f;

    for (const auto& system : systems)
    {
        int measureIndex = GetFirstMeasureInSystem(systemIndex);
        int lastInstrumentIndex = (int)instruments.size() - 1;

        if (measureIndex >= GetMeasureCount())
        {
            LOGW("measureIndex: %d, count: %d", measureIndex, GetMeasureCount());
            break;
        }

        if (DoesMeasureStartNewPage(measureIndex)) // if it is the first system on the page
        {
            systemPosition.y = displayConstants.topMargin + system.layout.topSystemDistance;
        }
        else
        {
            float lastInstrumentPositionY = GetInstrumentPositionY(measureIndex, lastInstrumentIndex);

            systemPosition.y = previousSystemPositionY + lastInstrumentPositionY + system.layout.systemDistance + instruments[lastInstrumentIndex]->GetMiddleHeight(10.0f, 13.33f, measureIndex, measureIndex + 1);
        }

        systemPosition.x = displayConstants.leftMargin + system.layout.systemLeftMargin;

        systemPositions.push_back(systemPosition);

        previousSystemPositionY = systemPosition.y;
        systemIndex++;
    }

    return systemPositions;
}

std::shared_ptr<Measure> Song::GetMeasureAtPoint(Vec2<float> point) const
{
    for (auto instrument : instruments)
    {
        for (auto staff: instrument->staves)
        {
            int measureIndex = 0;
            for (auto measure: staff->measures)
            {
                if (measure->boundingBox.DoesOverlapWithPoint(point))
                    return measure;
            }
        }
    }

    return nullptr;
}