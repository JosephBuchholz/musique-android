#include "SongData.h"
#include "../AndroidDebug.h"

Instrument* SongData::GetInstrument(const std::string& id) {
    for (auto& inst : instruments) {
        if (inst->id == id) {
            return inst;
        }
    }

    return nullptr;
}

void SongData::OnUpdate()
{
    LOGD("updating song data");

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
        for (Note *note: orderedNotes) {
            float noteStartX = -1.0f;
            float noteStopX = -1.0f;
            float position = 0.0f;
            float time = 0.0f;

            NoteData previousNoteData = NoteData(0.0f, 0.0f, 0.0f, 0);

            std::vector<NoteData*> noteDatas;
            for (auto& minNoteData: m_MinNoteData) {
                if (minNoteData.beatPositionInSong == note->beatPositionInSong && minNoteData.duration == note->duration.duration) // the same note
                    break;
                if (minNoteData.beatPositionInSong >= note->beatPositionInSong + note->duration.duration) // noteData.beatPositionInSong is past(or equal to) note stop beat position
                {
                    float stopBeatPosition = note->beatPositionInSong + note->duration.duration;
                    // calculates how much of the previous note's width should be taken away from the position
                    float diff = (stopBeatPosition - previousNoteData.beatPositionInSong); // num of beats between beatPositionInSong and previousNoteBeatPosition
                    float howMuch = (diff/previousNoteData.duration); // the percentage of previousNoteWidth that needs to be added to previousNoteX to get the x of beatPositionInSong
                    float howMuchToSub = 1.0f - howMuch; // the percentage of previousNoteWidth that needs to be subtracted from currentNoteX to get the x of beatPositionInSong
                    noteStopX = position - (previousNoteData.width * howMuchToSub); // get the percentage of previousNoteWidth and subtract
                    break;
                }
                else if (minNoteData.beatPositionInSong >= note->beatPositionInSong) // noteData.beatPositionInSong is past(or equal to) note->beatPositionInSong(what we want the x position of)
                {
                    if (minNoteData.beatPositionInSong != note->beatPositionInSong)
                        noteDatas.push_back(&previousNoteData);
                    noteDatas.push_back(&minNoteData);

                    if (noteStartX == -1.0f)
                    {
                        if (previousNoteData.duration == 0.0f) // noteData is the first in the list so the position is just 0.0f
                        {
                            break; // done
                        }

                        // calculates how much of the previous note's width should be taken away from the position
                        float diff = (note->beatPositionInSong - previousNoteData.beatPositionInSong); // num of beats between beatPositionInSong and previousNoteBeatPosition
                        float howMuch = (diff/previousNoteData.duration); // the percentage of previousNoteWidth that needs to be added to previousNoteX to get the x of beatPositionInSong
                        float howMuchToSub = 1.0f - howMuch; // the percentage of previousNoteWidth that needs to be subtracted from currentNoteX to get the x of beatPositionInSong
                        noteStartX = position - (previousNoteData.width * howMuchToSub); // get the percentage of previousNoteWidth and subtract
                    }
                }

                position += minNoteData.width;
                time += minNoteData.duration;
                previousNoteData.beatPositionInSong = minNoteData.beatPositionInSong;
                previousNoteData.duration = minNoteData.duration;
                previousNoteData.width = minNoteData.width;
                /*if (note->beatPositionInSong >= minNoteData.beatPositionInSong && note->beatPositionInSong <= minNoteData.beatPositionInSong + minNoteData.duration) {
                    float pos = note->beatPositionInSong
                }
                position*/
            }

            float noteWidth = noteStopX - noteStartX;
            float minNoteWidth = GetNoteMinWidthInFront(note);
            LOGW("noteWidth: %f, minNoteWidth: %f", noteWidth, minNoteWidth);
            if (noteWidth < minNoteWidth) // then fix it so that noteWidth >= minNoteWidth
            {
                LOGE("noteWidth < minNoteWidth %i", noteDatas.size());
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
                    LOGD("pw = %f; w = %f; howMuchToAdd = %f; pw * howMuchToAdd = %f", pw, w, howMuchToAdd, pw * howMuchToAdd);
                }
            }
        }
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
                measureIndex = noteData.measureIndex;
                measuresNotesWidths.push_back(width);
                width = 0.0f;
            }

            if (noteIndex >= orderedNotes.size()-1) // is last note
            {
                measuresNotesWidths.push_back(width);
            }

            width += noteData.width;
        }
    }

    // calculate measure widths
    for (auto* instrument : instruments) {
        for (auto* staff : instrument->staves) {
            int measureIndex = 0;
            for (auto* measure : staff->measures) {
                // these were ints (this comment is only here be cause I could have cause a bug)
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
        LOGD("width: %f pos: %f", m_MeasureWidths[i], GetMeasurePositionX(i));
    }

    LOGD("done updating song data");
}

void SongData::CalculateNoteBeatPositionsInSong()
{
    for (auto* instrument : instruments) {
        for (auto* staff : instrument->staves) {
            int measureIndex = 0;
            for (auto* measure : staff->measures) {
                float measureBeatPosition = staff->GetMeasureBeatPosition(measureIndex);

                for (Note* note : measure->notes) {
                    note->beatPositionInSong = note->beatPosition + measureBeatPosition;
                }

                measureIndex++;
            }
        }
    }
}

float SongData::GetNoteMinWidthInFront(Note* note)
{
    return note->GetMinWidth();
}

float SongData::GetNoteMinWidthBehind(Note* note)
{
    return 0.0f;
}

float SongData::GetMeasureWidth(int measureIndex)
{
    if (m_MeasureWidths.size() > measureIndex) {
        return m_MeasureWidths[measureIndex];
    }
    return 0.0f;
}

float SongData::GetMeasurePositionX(int measureIndex)
{
    float position = 0.0f;
    for (int i = 0; i < measureIndex; i++)
    {
        position += m_MeasureWidths[i];
    }
    return position;
}

float SongData::GetMeasureBeatPosition(int measureIndex)
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

float SongData::GetMeasurePositionY(int measureIndex)
{
    return 0.0f;
}

float SongData::GetSongWidth()
{
    float songWidth = 0.0f;
    for (float w : m_MeasureWidths)
    {
        songWidth += w;
    }
    return songWidth;
}

float SongData::GetPositionXInSong(float beatPositionInSong, int currentMeasureIndex)
{
    float position = 0.0f;

    float measurePositionX = GetMeasurePositionX(currentMeasureIndex);
    position += measurePositionX;
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