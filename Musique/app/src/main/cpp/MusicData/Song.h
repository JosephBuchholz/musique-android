#ifndef MUSIQUE_SONG_H
#define MUSIQUE_SONG_H

#include "Instrument.h"
#include <vector>
#include <string>
#include <array>
#include "../Debuging/AndroidDebug.h"
#include "SongData.h"
#include "../Settings.h"
#include "Credit.h"
#include "System.h"

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

    /**
     * Gets the width of the song (for Horizontal mode).
     *
     * @return The width of the song.
     */
    float GetSongWidth();

    /**
     * Converts a beatPosition into a normal x position.
     *
     * @param beatPositionInSong the beat position to be converted
     * @param currentMeasureIndex the index of the measure that the beat position is at
     * @return the x position in tenths
     */
    float GetPositionXInSong(float beatPositionInSong, int currentMeasureIndex);

    /**
     * Converts a beatPosition into a normal x position relative to the left-hand side of the measure.
     *
     * @param beatPositionInSong the beat position to be converted
     * @param currentMeasureIndex the index of the measure that the beat position is at
     * @return the x position in tenths
     */
    float GetPositionXInMeasure(float beatPositionInSong, int currentMeasureIndex);

    /**
     * Gets the y position of a system
     *
     * @param measureIndex the index of the first measure in the system
     * @return the y position of the system
     */
    float GetSystemPositionY(int measureIndex);

    /**
     * Calculates the height of a system (not implemented yet)
     *
     * @param measureIndex the index of the measure
     * @return the height of the system
     */
    float GetSystemHeight(int measureIndex);

    /**
     * Finds the index of the system that the given measure appears on.
     *
     * @param measureIndex the index of the measure
     * @return the index of the system
     */
    int GetSystemIndex(int measureIndex);

    /**
     * Finds the index of the page that the given measure appears on.
     *
     * @param measureIndex the index of the measure
     * @return the index of the page
     */
    int GetPageIndex(int measureIndex);

    /**
     * Finds the index of the first measure that appears on the given page index
     *
     * @param pageIndex the index of the page
     * @return the index of the measure
     */
    int GetFirstMeasureOnPage(int pageIndex);
    int GetFirstMeasureInSystem(int systemIndex);

    /**
     * Calculates the number of pages needed to render in paged mode.
     *
     * @return the number of pages
     */
    int GetNumPages();

    /**
     * Finds the minimum amount of space needed in front of the note.
     *
     * @param note A pointer to the note.
     * @return The amount of space in front of the note.
     */
    float GetNoteMinWidthInFront(Note* note);

    /**
     * Finds the minimum amount of space needed behind the note.
     *
     * @param note A pointer to the note.
     * @return The amount of space behind the note.
     */
    float GetNoteMinWidthBehind(Note* note);

    /**
     * Gets the measure at the given index (this will be the measure in the first instrument and staff).
     *
     * @param measureIndex the index of the measure
     * @return a pointer to the measure
     */
    Measure* GetMeasure(int measureIndex);

    /**
     * Finds whether the measure at the given index is at the start of the system (i.e. it starts a new system).
     *
     * @param measureIndex the index of the measure.
     * @return weather the measure starts a new system.
     */
    bool DoesMeasureStartNewSystem(int measureIndex);

    /**
     * Finds whether the measure at the given index is at the start of the page (i.e. it starts a new page).
     *
     * @param measureIndex the index of the measure.
     * @return weather the measure starts a new page.
     */
    bool DoesMeasureStartNewPage(int measureIndex);

    /**
     * Gets the x position of a measure at the given index.
     *
     * @param measureIndex The index of the measure to find the x position of.
     * @param pagePosition The position of the page that the measure is on.
     * @param systemPosition The position of the system that the system is in.
     * @return The x position of the measure.
     */
    float GetMeasurePositionX(int measureIndex, Vec2<float> pagePosition, Vec2<float> systemPosition);

    /**
     * Updates the position and dimensions of all bounding boxes.
     *
     * @param pagePositions A list of positions for all pages.
     * @param systemPositions A list of positions for all systems.
     */
    void UpdateBoundingBoxes(const std::vector<Vec2<float>>& pagePositions, const std::vector<Vec2<float>>& systemPositions);

    /**
     * Renders a debug version of every bounding box. (This function is for debugging purposes).
     *
     * @param renderData The `RenderData` object to render to.
     * @param pagePositions A list of positions for all pages.
     * @param systemPositions A list of positions for all systems.
     */
    void RenderBoundingBoxes(RenderData& renderData, const std::vector<Vec2<float>>& pagePositions, const std::vector<Vec2<float>>& systemPositions);

    /**
     * Calculates the instrument's y position relative to the current system.
     *
     * @param measureIndex The index of the measure.
     * @param instrumentIndex The index of the instrument.
     * @return The instrument's y position relative to the current system.
     */
    float GetInstrumentPositionY(int measureIndex, int instrumentIndex);

private:
    struct TimeSpacePoint
    {
        float beatPositionInSong = 0.0f;
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

    //std::vector<System::SystemLayout> systemLayouts;

    std::vector<Instrument*> instruments;
    std::vector<System> systems;

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

#endif // MUSIQUE_SONG_H