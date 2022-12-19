#include "Song.h"
#include "SongData.h"
#include "../AndroidDebug.h"

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
        LOGE("paged");
        // calculate positions
        for (auto* instrument : instruments)
        {
            for (auto* staff : instrument->staves)
            {
                int measureIndex = 0;
                for (auto* measure : staff->measures)
                {
                    measure->CalculateWidthAsPaged();
                    if (measureIndex >= m_MeasureWidths.size())
                    {
                        m_MeasureBeginWidths.push_back(0.0f);
                        m_MeasureWidths.push_back(measure->measureWidth);
                    }
                    LOGE("width: %f, default: %f, index: %d", measure->measureWidth, measure->defaultMeasureWidth, measureIndex);

                    for (auto* note : measure->notes)
                    {
                        note->CalculatePositionAsPaged(settings.displayConstants);
                    }

                    measureIndex++;
                }
            }
        }
    }
    else if (settings.musicLayout == Settings::MusicLayout::Vertical || settings.musicLayout == Settings::MusicLayout::Horizontal)
    {
        LOGE("vertical or horizontal");
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
                                ls = settings.displayConstants.tabLineSpacing;
                            else
                                ls = settings.displayConstants.lineSpacing;
                            note->positionX = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->positionY = ((ls * float(staff->lines - 1)) / 2.0f);
                        }
                        else if (note->type == Note::NoteType::Tab) // is a tab note
                        {
                            note->positionX = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->positionY = (settings.displayConstants.tabLineSpacing * float(note->string - 1));
                        }
                        else // is a standard note
                        {
                            note->positionX = GetPositionXInMeasure(note->beatPositionInSong, measureIndex);
                            note->positionY = (settings.displayConstants.lineSpacing * measure->CalculateNoteYPositionRelativeToMeasure(noteIndex));
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

float Song::GetPositionXInMeasure(float beatPositionInSong, int currentMeasureIndex)
{
    float position = 0.0f;

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
    for (auto noteData : m_MinNoteData)
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
                float howMuch = (diff/previousNoteDuration); // the percentage of previousNoteWidth that needs to be added to previousNoteX to get the x of beatPositionInSong
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

    return position;
}

float Song::GetPositionXInSong(float beatPositionInSong, int currentMeasureIndex)
{
    float position = GetPositionXInMeasure(beatPositionInSong, currentMeasureIndex);

    float measurePositionX = GetMeasurePositionX(currentMeasureIndex);
    position += measurePositionX;

    return position;
}

Measure* Song::GetMeasure(int measureIndex)
{
    if (!instruments.empty())
        if (!instruments[0]->staves.empty())
            if (instruments[0]->staves[0]->measures.size() > measureIndex)
                return instruments[0]->staves[0]->measures[measureIndex];
    return nullptr;
}

bool Song::DoesMeasureStartNewSystem(int measureIndex)
{
    Measure* measure = GetMeasure(measureIndex);
    return measure->startNewSystem;
}

bool Song::DoesMeasureStartNewPage(int measureIndex)
{
    Measure* measure = GetMeasure(measureIndex);
    return measure->startNewPage;
}

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

float Song::GetSystemHeight(int measureIndex)
{
    return 150.0f;
}