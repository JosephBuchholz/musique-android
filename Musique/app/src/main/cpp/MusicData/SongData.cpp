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
                m_MinNoteData.emplace_back(note->GetMinWidth(), note->beatPositionInSong, note->duration.duration );

                if (measureIndex != note->measureIndex) {
                    measureIndex = note->measureIndex;
                    measuresNotesWidths.push_back(width);
                    width = 0.0f;
                }

                width += note->GetMinWidth(); // moved
            }

            if (noteIndex >= orderedNotes.size()-1) // is last note
            {
                //width += note->GetMinWidth();
                measuresNotesWidths.push_back(width);
            }

            noteIndex++;
        }
        LOGD("time: %f", time);
        LOGD("steps: %i", steps);
    }

    // calculate measure widths
    for (auto* instrument : instruments) {
        for (auto* staff : instrument->staves) {
            int measureIndex = 0;
            for (auto* measure : staff->measures) {
                int minWidth = measure->CalculateMinWidth(measuresNotesWidths[measureIndex]);
                int beginWidth = measure->GetBeginningWidth();

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

    float pos = 0.0f;
    float time = 0.0f;
    float previousNoteWidth = 0.0f;
    float previousNoteBeatPosition = 0.0f;
    float previousNoteDuration = 0.0f;
    for (auto noteData : m_MinNoteData)
    {
        if (time >= GetMeasureBeatPosition(currentMeasureIndex)) // should be equal first time
        {
            if (previousNoteBeatPosition == 0.0f)
            {
                previousNoteBeatPosition = GetMeasureBeatPosition(currentMeasureIndex);
            }
            //previousNoteBeatPosition = GetMeasureBeatPosition(currentMeasureIndex);

            if (currentMeasureIndex == 1) {
                //LOGD("beat positions: %f, %f, %i, %f", noteData[1], beatPositionInSong, currentMeasureIndex, noteData[2]);
            } else if (currentMeasureIndex == 0)
            {
                //LOGD("beat positions at measure 0: %f, %f, %i, %f", noteData[1], beatPositionInSong, currentMeasureIndex, noteData[2]);
            }
            if (noteData.beatPosition/*beat position*/ >= beatPositionInSong)
            {
                if (previousNoteDuration == 0.0f)
                {
                    break;
                }
                // calculates how much of the previous note's width should be taken away from the position



                float diff = (beatPositionInSong - previousNoteBeatPosition);
                float howMuch = (diff/previousNoteDuration);
                float howMuchToSub = 1.0f - howMuch;
                position -= (previousNoteWidth * howMuchToSub); // scale?
                //position -= previousNoteWidth - (previousNoteWidth * ((beatPositionInSong - previousNoteBeatPosition)/previousNoteDuration));
                break; // done
            }
            position += noteData.width/*width*/;
            previousNoteBeatPosition = noteData.beatPosition/*beat position*/;
            previousNoteDuration = noteData.duration/*duration*/;
        }
        pos += noteData.width/*width*/;
        time += noteData.duration/*duration*/;
        previousNoteWidth = noteData.width/*width*/;
    }

    return position;
}