#include "Song.h"
#include "SongData.h"
#include "../AndroidDebug.h"
#include "../Utils/Converters.h"

#include <algorithm>

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

Instrument* Song::GetInstrument(const std::string& id) {
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
    for (auto* instrument : instruments) {
        InstrumentInfo instInfo = InstrumentInfo();
        instInfo.name = instrument->name.string;
        instInfo.volume = 100;
        instInfo.visible = true;
        songData.instrumentInfos.push_back(instInfo);
    }

    // stem and beam positions
    /*struct BeamGroup
    {
        int voice = 0;
        int level = 1;
        std::vector<Note*> notes;
    };
    std::vector<BeamGroup> beamGroups;
    std::vector<BeamGroup> finalBeamGroups;
    Note* stemNote = nullptr;
    for (auto* instrument : instruments) {
        for (auto* staff : instrument->staves) {
            for (auto* measure : staff->measures) {
                for (auto* note : measure->notes) {

                    if (stemNote != nullptr)
                    {
                        if (stemNote->beatPositionInSong == note->beatPosition)
                        {
                            if (stemNote->noteStem.stemType == NoteStem::StemType::Up && Note::IsNoteIsHigher(note, stemNote))
                            {

                            }
                            else
                            {

                            }
                        }
                    }

                    if (note->beam.beamType != Beam::BeamType::None)
                    {
                        bool foundGroup = false;
                        int i = 0;
                        for (auto group: beamGroups) {
                            if (group.voice == note->voice && group.level == note->beam.beamLevel) {
                                switch (note->beam.beamType)
                                {
                                    case Beam::BeamType::Begin:
                                    {
                                        BeamGroup newGroup;
                                        newGroup.voice = note->voice;
                                        newGroup.level = note->beam.beamLevel;
                                        newGroup.notes.push_back(note);
                                        beamGroups.push_back(newGroup);
                                        break;
                                    }
                                    case Beam::BeamType::End: {
                                        group.notes.push_back(note);
                                        finalBeamGroups.push_back(group);
                                        beamGroups.erase(beamGroups.begin() + i);
                                        break;
                                    }
                                    case Beam::BeamType::Continue: {
                                        group.notes.push_back(note);
                                        break;
                                    }
                                    case Beam::BeamType::ForwardHook: {
                                        group.notes.push_back(note);
                                        break;
                                    }
                                    case Beam::BeamType::BackwardHook: {
                                        group.notes.push_back(note);
                                        break;
                                    }
                                    default: { break; }
                                }

                                foundGroup = true;
                                break;
                            }
                            i++;
                        }

                        if (!foundGroup) {
                            if (note->beam.beamType == Beam::BeamType::Begin)
                            {
                                BeamGroup newGroup;
                                newGroup.voice = note->voice;
                                newGroup.level = note->beam.beamLevel;
                                newGroup.notes.push_back(note);
                                beamGroups.push_back(newGroup);
                            }
                            else
                            {
                                LOGE("ERROR WITH BEAM AND STEAM CODE IN SONGDATA.CPP");
                            }
                        }
                    }
                }
            }
        }
    }

    for (auto group: finalBeamGroups)
    {
        int i = 0;
        for (auto* note: group.notes)
        {


            i++;
        }
    }*/

    // tie calculations
    for (auto* instrument : instruments)
    {
        for (auto* staff : instrument->staves)
        {
            std::vector<Note*> tiedStartNotes;

            int measureIndex = 0;
            for (auto* measure : staff->measures)
            {
                int noteIndex = 0;
                for (auto* note : measure->notes)
                {
                    //LOGV("s: %d", tiedStartNotes.size());
                    if (note->tie.type == NoteTie::TieType::Start)
                    {
                        LOGW("Tie start: s: %d", tiedStartNotes.size());
                        tiedStartNotes.push_back(note);
                    }
                    else if (note->tie.type == NoteTie::TieType::Stop)
                    {
                        if (!tiedStartNotes.empty())
                        {
                            LOGW("Tie end: s: %d", tiedStartNotes.size());

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
    }

    // beam calculations
    for (auto* instrument : instruments)
    {
        for (auto* staff : instrument->staves)
        {
            int measureIndex = 0;
            for (auto* measure : staff->measures)
            {
                int noteIndex = 0;
                const int maxBeamLevel = 4; // TODO: probably should be fixed
                std::array<Beam, maxBeamLevel> beams; // temp to store/keep track of beams
                BeamGroup beamGroup; // temp to store/keep track of a beam group

                bool beamGroupIsDone = false;

                for (auto* note : measure->notes)
                {
                    for (const NoteBeamData& noteBeamData : note->beamData)
                    {
                        if (noteBeamData.beamType == NoteBeamData::BeamType::Begin)
                        {
                            if (noteBeamData.beamLevel == 1) // start a new beam group
                            {
                                beamGroup = BeamGroup();

                                if (note->noteStem.stemType == NoteStem::StemType::Up)
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
    for (auto* instrument : instruments) {
        for (auto* staff : instrument->staves) {
            totalBeatWidth = staff->GetTotalBeatWidth();
            int measureIndex = 0;
            for (auto* measure : staff->measures) {
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

        // calculate positions for notes and measures
        for (auto* instrument : instruments)
        {
            for (auto* staff : instrument->staves)
            {
                staff->CalculateAsPaged(displayConstants);

                int measureIndex = 0;
                int systemIndex = 0;
                for (auto* measure : staff->measures)
                {
                    measure->CalculateAsPaged(displayConstants);
                    if (measureIndex >= m_MeasureWidths.size())
                    {
                        m_MeasureBeginWidths.push_back(0.0f);
                        m_MeasureWidths.push_back(measure->measureWidth);
                    }

                    if (staff->type == Staff::StaffType::Tab)
                        measure->showKeySignature = false;

                    m_MeasureBeginWidths[measureIndex] = std::max(measure->GetBeginningWidth(), m_MeasureBeginWidths[measureIndex]);

                    systems[systemIndex].clefPositionX = std::max(measure->GetClefPositionInMeasure(systems[systemIndex]), systems[systemIndex].clefPositionX);
                    systems[systemIndex].keySignaturePositionX = std::max(measure->GetKeySignaturePositionInMeasure(systems[systemIndex]), systems[systemIndex].keySignaturePositionX);
                    systems[systemIndex].timeSignaturePositionX = std::max(measure->GetTimeSignaturePositionInMeasure(systems[systemIndex]), systems[systemIndex].timeSignaturePositionX);

                    int noteIndex = 0;
                    for (auto* note : measure->notes)
                    {
                        if (!note->isRest) // musescore specific code
                        {
                            note->CalculatePositionAsPaged(displayConstants);

                            if (note->type == Note::NoteType::Standard)
                                note->positionY = (displayConstants.lineSpacing * measure->CalculateNoteYPositionRelativeToMeasure(noteIndex));
                        }

                        for (auto& lyric : note->lyrics)
                        {
                            lyric.CalculatePositionAsPaged(displayConstants);
                        }

                        noteIndex++;
                    }

                    measureIndex++;

                    if (measure->startNewSystem)
                        systemIndex++;
                }
            }
        }

        // sort all the notes by their beat positions and put them in an array of time space points
        m_TimeSpacePoints.clear();
        for (auto* instrument : instruments) {
            for (auto* staff : instrument->staves) {
                int measureIndex = 0;
                for (auto* measure : staff->measures) {
                    TimeSpacePoint beginMeasurePoint = TimeSpacePoint();
                    beginMeasurePoint.beatPosition = measure->beatPosition;
                    beginMeasurePoint.position = 0.0f;
                    beginMeasurePoint.measureIndex = measureIndex;
                    TimeSpacePoint endMeasurePoint = TimeSpacePoint();
                    endMeasurePoint.beatPosition = measure->beatPosition + measure->duration.duration;
                    endMeasurePoint.position = measure->measureWidth;
                    endMeasurePoint.measureIndex = measureIndex;

                    //AddTimeSpacePoint(beginMeasurePoint);
                    AddTimeSpacePoint(endMeasurePoint);

                    for (Note* note : measure->notes) {

                        if (note->isRest)
                            continue;

                        int insertIndex = 0;
                        bool added = false;
                        for (TimeSpacePoint point : m_TimeSpacePoints) {
                            if (note->beatPositionInSong < point.beatPosition) // note should come before this point
                            {
                                TimeSpacePoint newPoint = TimeSpacePoint();
                                newPoint.beatPosition = note->beatPositionInSong;
                                newPoint.position = note->positionX;
                                newPoint.measureIndex = measureIndex;
                                m_TimeSpacePoints.insert(m_TimeSpacePoints.begin() + insertIndex, newPoint); // insert note at insertIndex
                                added = true;
                                break;
                            }
                            else if (note->beatPositionInSong == point.beatPosition) // note is not needed to make a new point
                            {
                                added = true;
                                break;
                            } // else note should come after orderedNote

                            insertIndex++;
                        }

                        if (!added)
                        {
                            TimeSpacePoint newPoint = TimeSpacePoint();
                            newPoint.beatPosition = note->beatPositionInSong;
                            newPoint.position = note->positionX;
                            newPoint.measureIndex = measureIndex;
                            m_TimeSpacePoints.push_back(newPoint);
                        }
                    }

                    measureIndex++;
                }
            }
        }

        // calculate positions for everything else
        for (auto* instrument : instruments)
        {
            for (auto* staff : instrument->staves)
            {
                int measureIndex = 0;
                for (auto* measure : staff->measures)
                {
                    for (auto& direction : measure->directions)
                    {
                        float defaultX = GetPositionXInMeasure(direction.beatPositionInSong, measureIndex);
                        float defaultY = -20.0f;

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
                    }

                    for (auto& chord : measure->chords)
                    {
                        float defaultX = GetPositionXInMeasure(chord.beatPositionInSong, measureIndex);
                        float defaultY = -40.0f;

                        chord.CalculatePositionAsPaged(displayConstants, defaultX, defaultY);
                    }

                    for (auto* note : measure->notes)
                    {
                        if (note->isRest) // musescore specific code
                        {
                            note->positionX = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            //LOGE("Rest Position X: %f, BP: %f, m: %d", note->positionX, note->beatPositionInSong, measureIndex);
                            note->positionY = 0.0f;
                        }
                    }


                    // calculate beam positions
                    for (BeamGroup& beamGroup : measure->beams)
                    {
                        if (!beamGroup.notes.empty())
                        {
                            Note* firstNote = beamGroup.notes[0];
                            Note* lastNote = beamGroup.notes[beamGroup.notes.size() - 1];

                            beamGroup.beamStartPositionX = firstNote->positionX + firstNote->noteStem.stemPositionX;
                            beamGroup.beamStartPositionY = firstNote->positionY + firstNote->noteStem.stemEndY;

                            beamGroup.beamEndPositionX = lastNote->positionX + lastNote->noteStem.stemPositionX;
                            beamGroup.beamEndPositionY = lastNote->positionY + lastNote->noteStem.stemEndY;
                        }

                        for (Beam& beam : beamGroup.beams)
                        {
                            if (!beam.notes.empty())
                            {
                                Note* firstNote = beam.notes[0];
                                Note* lastNote = beam.notes[beam.notes.size() - 1];

                                beam.beamStartPositionX = firstNote->positionX + firstNote->noteStem.stemPositionX;
                                beam.beamStartPositionY = beamGroup.GetPositionYOnBeam(firstNote->positionX);

                                beam.beamEndPositionX = lastNote->positionX + lastNote->noteStem.stemPositionX;
                                beam.beamEndPositionY = beamGroup.GetPositionYOnBeam(lastNote->positionX);
                            }
                            else
                                LOGE("Beam has no notes associated with it");
                        }

                        // calculate stem lengths
                        for (Note* beamNote : beamGroup.notes)
                        {
                            float beamPositionYAtNote = beamGroup.GetPositionYOnBeam(beamNote->positionX);
                            beamNote->noteStem.stemEndY = beamPositionYAtNote - beamNote->positionY;
                        }
                    }

                    measureIndex++;
                }
            }
        }
    }
    else if (settings.musicLayout == Settings::MusicLayout::Vertical || settings.musicLayout == Settings::MusicLayout::Horizontal)
    {
        // sort all the notes by their stop times(beatPositionInSong + duration) from smallest to largest
        std::vector<Note*> orderedNotes;
        for (auto* instrument : instruments) {
            for (auto* staff : instrument->staves) {
                int measureIndex = 0;
                for (auto* measure : staff->measures) {
                    float measureBeatPosition = staff->GetMeasureBeatPosition(measureIndex);

                    for (Note* note : measure->notes) {
                        float noteBeatPosition = note->beatPosition + measureBeatPosition;
                        float noteBeatLength = note->duration.duration;

                        int insertIndex = 0;
                        for (Note* orderedNote : orderedNotes) {
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
            for (Note* note : orderedNotes) {
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
            for (Note *note: orderedNotes) {
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
                    /*previousNoteData.beatPositionInSong = minNoteData.beatPositionInSong;
                    previousNoteData.duration = minNoteData.duration;
                    previousNoteData.width = minNoteData.width;*/
                    /*if (note->beatPositionInSong >= minNoteData.beatPositionInSong && note->beatPositionInSong <= minNoteData.beatPositionInSong + minNoteData.duration) {
                        float pos = note->beatPositionInSong
                    }
                    position*/
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
        for (auto* instrument : instruments) {
            for (auto* staff : instrument->staves) {
                int measureIndex = 0;
                for (auto* measure : staff->measures) {
                    // these were ints (this comment is only here be cause I could have caused a bug)
                    float minWidth = measure->CalculateMinWidth(measuresNotesWidths[measureIndex]);
                    float beginWidth = measure->GetBeginningWidth();

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

        for (auto* instrument : instruments)
        {
            for (auto* staff : instrument->staves)
            {
                int measureIndex = 0;
                for (auto* measure : staff->measures)
                {
                    measure->measureWidth = GetMeasureWidth(measureIndex);

                    int noteIndex = 0;
                    for (auto* note : measure->notes)
                    {
                        if (note->isRest) { // is a rest
                            float ls;
                            if (note->type == Note::NoteType::Tab)
                                ls = displayConstants.tabLineSpacing;
                            else
                                ls = displayConstants.lineSpacing;
                            note->positionX = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->positionY = ((ls * float(staff->lines - 1)) / 2.0f);
                        }
                        else if (note->type == Note::NoteType::Tab) // is a tab note
                        {
                            note->positionX = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->positionY = (displayConstants.tabLineSpacing * float(note->string - 1));
                        }
                        else // is a standard note
                        {
                            note->positionX = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->positionY = (displayConstants.lineSpacing * measure->CalculateNoteYPositionRelativeToMeasure(noteIndex));
                        }

                        noteIndex++;
                    }

                    measureIndex++;
                }
            }
        }
    }

    LOGD("done updating song data");
}

void Song::AddTimeSpacePoint(TimeSpacePoint point)
{
    bool added = false;
    int insertIndex = 0;
    for (TimeSpacePoint compare : m_TimeSpacePoints) {
        if (point.beatPosition < compare.beatPosition) // point should come before compare
        {
            m_TimeSpacePoints.insert(m_TimeSpacePoints.begin() + insertIndex, point); // insert point at insertIndex
            added = true;
            break;
        }
        else if (point.beatPosition == compare.beatPosition) // point is not needed
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
    for (auto* instrument : instruments) {
        for (auto* staff : instrument->staves) {
            int measureIndex = 0;
            for (auto* measure : staff->measures) {
                float measureBeatPosition = staff->GetMeasureBeatPosition(measureIndex);

                for (Note* note : measure->notes) {
                    note->beatPositionInSong = note->beatPosition + measureBeatPosition;
                }

                for (Chord& chord : measure->chords) {
                    chord.beatPositionInSong = chord.beatPosition + measureBeatPosition;
                }

                measureIndex++;
            }
        }
    }
}

float Song::GetNoteMinWidthInFront(Note* note)
{
    return note->GetMinWidth();
}

float Song::GetNoteMinWidthBehind(Note* note)
{
    return 0.0f;
}

float Song::GetMeasureWidth(int measureIndex)
{
    if (m_MeasureWidths.size() > measureIndex) {
        return m_MeasureWidths[measureIndex];
    }
    return 0.0f;
}

float Song::GetMeasurePositionX(int measureIndex)
{
    float position = 0.0f;
    if (DoesMeasureStartNewSystem(measureIndex))
        return position;
    else
    {
        for (int i = 0; i < measureIndex; i++)
        {
            if (DoesMeasureStartNewSystem(i))
                position = 0.0f;
            position += m_MeasureWidths[i];
        }

        return position;
    }
}

float Song::GetMeasureBeatPosition(int measureIndex)
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

float Song::GetMeasurePositionY(int measureIndex)
{
    return 0.0f;
}

float Song::GetSongWidth()
{
    float songWidth = 0.0f;
    for (float w : m_MeasureWidths)
    {
        songWidth += w;
    }
    return songWidth;
}

/**
 * Converts a beatPosition into a normal x position relative to the left-hand side of the measure.
 *
 * @param beatPositionInSong the beat position to be converted
 * @param currentMeasureIndex the index of the measure that the beat position is at
 * @return the x position in tenths
 */
float Song::GetPositionXInMeasure(float beatPositionInSong, int currentMeasureIndex)
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
                if (FloatsAreEqual(beatPositionInSong, point.beatPosition))
                {
                    position = point.position;
                    //LOGE("beatPositionInSong == point.beatPosition: %f", position);
                    //LOGE("beatPositionInSong == point.beatPosition: bpos: %f, pos: %f, pointBP: %f, prePointBP: %f, pMI: %d, prePMI: %d, pointP: %f, prePointP: %f", beatPositionInSong, position, point.beatPosition, previousPoint.beatPosition, point.measureIndex, previousPoint.measureIndex, point.position, previousPoint.position);

                    break;
                }
                else if (beatPositionInSong < point.beatPosition)
                {
                    float previousBeatPosition = previousPoint.beatPosition;
                    if (currentMeasureIndex != previousPoint.measureIndex)
                        previousBeatPosition = 0.0f;

                    float posRange = (point.beatPosition - previousBeatPosition);
                    if (posRange == 0)
                        LOGE("divide by 0!!!!!!!!!!");
                    float percentage = (beatPositionInSong - previousBeatPosition) / posRange;
                    position = (point.position - previousPoint.position) * percentage;
                    position += previousPoint.position;

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

/**
 * Converts a beatPosition into a normal x position.
 *
 * @param beatPositionInSong the beat position to be converted
 * @param currentMeasureIndex the index of the measure that the beat position is at
 * @return the x position in tenths
 */
float Song::GetPositionXInSong(float beatPositionInSong, int currentMeasureIndex)
{
    float position = GetPositionXInMeasure(beatPositionInSong, currentMeasureIndex);

    float measurePositionX = GetMeasurePositionX(currentMeasureIndex);
    position += measurePositionX;

    return position;
}

/**
 * Gets the measure at the given index (this will be the measure in the first instrument and staff).
 *
 * @param measureIndex the index of the measure
 * @return a pointer to the measure
 */
Measure* Song::GetMeasure(int measureIndex)
{
    if (!instruments.empty())
        if (!instruments[0]->staves.empty())
            if (instruments[0]->staves[0]->measures.size() > measureIndex)
                return instruments[0]->staves[0]->measures[measureIndex];
    return nullptr;
}

/**
 * Finds whether the measure at the given index is at the start of the system (i.e. it starts a new system)
 *
 * @param measureIndex the index of the measure
 * @return weather the measure starts a new system
 */
bool Song::DoesMeasureStartNewSystem(int measureIndex)
{
    Measure* measure = GetMeasure(measureIndex);
    return measure->startNewSystem;
}

/**
 * Finds whether the measure at the given index is at the start of the page (i.e. it starts a new page)
 *
 * @param measureIndex the index of the measure
 * @return weather the measure starts a new page
 */
bool Song::DoesMeasureStartNewPage(int measureIndex)
{
    Measure* measure = GetMeasure(measureIndex);
    return measure->startNewPage;
}

/**
 * Gets the y position of a system
 *
 * @param measureIndex the index of the first measure in the system
 * @return the y position of the system
 */
float Song::GetSystemPositionY(int measureIndex) // TODO: needs finnished
{
    float instYPosition = 0.0f;
    int instrumentIndex = 0;
    Instrument* prevInstrument = nullptr;
    for (auto* instrument: instruments) {

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

/**
 * Calculates the height of a system (not implemented yet)
 *
 * @param measureIndex the index of the measure
 * @return the height of the system
 */
float Song::GetSystemHeight(int measureIndex)
{
    return 150.0f;
}

/**
 * Finds the index of the system that the given measure appears on.
 *
 * @param measureIndex the index of the measure
 * @return the index of the system
 */
int Song::GetSystemIndex(int measureIndex)
{
    int systemIndex = 0;

    if (instruments.empty())
        return 0;
    if (instruments[0]->staves.empty())
        return 0;

    for (auto* measure : instruments[0]->staves[0]->measures)
    {
        if (measure->startNewSystem)
            systemIndex++;

        if (measure->index == measureIndex)
            break;
    }

    return systemIndex;
}

/**
 * Finds the index of the page that the given measure appears on.
 *
 * @param measureIndex the index of the measure
 * @return the index of the page
 */
int Song::GetPageIndex(int measureIndex)
{
    int pageIndex = 0;

    if (instruments.empty())
        return 0;
    if (instruments[0]->staves.empty())
        return 0;

    for (auto* measure : instruments[0]->staves[0]->measures)
    {
        if (measure->startNewPage)
            pageIndex++;

        if (measure->index == measureIndex)
            break;
    }

    return pageIndex;
}

/**
 * Finds the index of the first measure that appears on the given page index
 *
 * @param pageIndex the index of the page
 * @return the index of the measure
 */
int Song::GetFirstMeasureOnPage(int pageIndex)
{
    int measureIndex = 0;

    if (instruments.empty())
        return 0;
    if (instruments[0]->staves.empty())
        return 0;

    int i = 0;
    for (auto* measure : instruments[0]->staves[0]->measures)
    {
        if (measure->startNewPage)
            i++;

        if (i == pageIndex)
            break;

        measureIndex++;
    }

    return measureIndex;
}

/**
 * Calculates the number of pages needed to render in paged mode.
 *
 * @return the number of pages
 */
int Song::GetNumPages()
{
    int numPages = 1;

    if (instruments.empty())
        return 0;
    if (instruments[0]->staves.empty())
        return 0;

    for (auto* measure : instruments[0]->staves[0]->measures)
    {
        if (measure->startNewPage)
            numPages++;
    }

    return numPages;
}