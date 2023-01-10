#pragma once
#include "Instrument.h"
#include <vector>
#include <string>
#include <array>
#include "../AndroidDebug.h"
#include "SongData.h"
#include "../Settings.h"
#include "Credit.h"

class Song {

public:

    Song();
    ~Song();

    Instrument* GetInstrument(const std::string& id);

    void OnLayoutChanged(Settings::MusicLayout layout);

    void OnUpdate();

    int GetMeasureCount() const { return m_MeasureWidths.size(); }
    float GetMeasureWidth(int measureIndex);
    float GetMeasurePositionX(int measureIndex);
    float GetMeasurePositionY(int measureIndex); // for use with a non horizontal mode (like a vertical mode)
    float GetMeasureBeatPosition(int measureIndex);

    float GetSongWidth(); // gets the width of the song

    float GetPositionXInSong(float beatPositionInSong, int currentMeasureIndex);
    float GetPositionXInMeasure(float beatPositionInSong, int currentMeasureIndex);

    float GetSystemPositionY(int measureIndex);
    float GetSystemHeight(int measureIndex);

    int GetSystemIndex(int measureIndex);
    int GetPageIndex(int measureIndex);

    int GetFirstMeasureOnPage(int pageIndex);

    int GetNumPages();

    float GetNoteMinWidthInFront(Note* note); // the minimum amount of space needed in front of the note
    float GetNoteMinWidthBehind(Note* note); // the minimum amount of space needed behind the note

    Measure* GetMeasure(int measureIndex);

    bool DoesMeasureStartNewSystem(int measureIndex);
    bool DoesMeasureStartNewPage(int measureIndex);

private:
    struct TimeSpacePoint
    {
        float beatPosition = 0.0f;
        float position = 0.0f;
        int measureIndex = 0;
    };

    void CalculateNoteBeatPositionsInSong();

    void AddTimeSpacePoint(TimeSpacePoint point);

public:

    MusicDisplayConstants displayConstants = MusicDisplayConstants();

    Settings settings;

    std::string musicXMLVersion = "1.0";

    SongData songData = SongData();

    std::vector<SystemLayout> systemLayouts;

    std::vector<Instrument*> instruments;

    std::vector<Credit> credits;

    // creators, like composers or arrangers
    struct Creator {
        std::string name = "";
        std::string strType = "";

        enum class CreatorType {
            None = 0, Composer, Lyricist, Arranger
        } type = CreatorType::None;
    };
    std::vector<Creator> creators;

    // rights, like copyright
    struct Rights {
        std::string right = "";
        std::string strType = "";

        enum class RightType {
            None = 0, Music, Words, Arrangement
        } type = RightType::None;
    };
    std::vector<Rights> rights;

    // encoders
    struct Encoder {
        std::string name = "";
        std::string strType = "";

        enum class EncoderType {
            None = 0, Music, Words, Arrangement
        } type = EncoderType::None;
    };
    std::vector<Encoder> encoders;

    std::string encodingDescription = "";

    std::string software = "";

    Date encodingDate = Date();

    float totalBeatWidth = 0.0f;

private:
    std::vector<float> m_MeasureWidths; // the widths of the measures
    std::vector<float> m_MeasureBeginWidths; // the widths of the measure's beginning width(before the notes, like the width of the time signature and key signature)

    struct NoteData
    {
        NoteData(float width, float beatPositionInSong, float duration, int measureIndex)
                : width(width), beatPositionInSong(beatPositionInSong), duration(duration), measureIndex(measureIndex) {}
        float width = 0.0f;
        float beatPositionInSong = 0.0f;
        float duration = 0.0f;
        int measureIndex = 0;
    };
    std::vector<NoteData> m_MinNoteData; // the widths, beat positions, and durations of the smallest notes

    std::vector<TimeSpacePoint> m_TimeSpacePoints;
};