#pragma once
#include "Instrument.h"
#include <vector>
#include <string>
#include <array>
#include "../AndroidDebug.h"

class SongData {

public:

    SongData();
    ~SongData()
    {
        LOGD("deconstructing song data");
        m_MeasureWidths.clear();
        m_MeasureBeginWidths.clear();
        m_MinNoteData.clear();
        LOGD("done deconstructing song data");
    }

    Instrument* GetInstrument(std::string id);

    void OnUpdate();

    float GetMeasureWidth(int measureIndex);
    float GetMeasurePositionX(int measureIndex);
    float GetMeasurePositionY(int measureIndex);
    float GetMeasureBeatPosition(int measureIndex);

    float GetSongWidth(); // gets the width of the song

    float GetPositionXInSong(float beatPositionInSong, int currentMeasureIndex);

private:
    void CalculateNoteBeatPositionsInSong();

public:
    std::string songTitle = "Title";

    std::string musicXMLVersion = "1.0";


    std::vector<Instrument*> instruments;

private:
    std::vector<float> m_MeasureWidths; // the widths of the measures
    std::vector<float> m_MeasureBeginWidths; // the widths of the measure's beginning width(before the notes, like the width of the time signature and key signature)

    std::vector<std::array<float, 3>> m_MinNoteData; // the widths, beat positions, and durations of the smallest notes
};