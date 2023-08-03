/**
 * This file contains the definition for the 'Song' class.
 */

#ifndef MUSIQUE_SONG_H
#define MUSIQUE_SONG_H

#include "Instrument.h"
#include <vector>
#include <string>
#include <array>
#include "../Debuging/AndroidDebug.h"
#include "SongData.h"
#include "../Settings.h"
#include "Page/Credit.h"
#include "System.h"
#include "Page/Page.h"
#include "Endings/EndingGroup.h"

/**
 * Represents and holds all the musical data for a song/piece.
 */
class Song
{

public:

    ~Song();

    std::shared_ptr<Instrument> GetInstrument(const std::string& id) const;

    void OnLayoutChanged(Settings::MusicLayout layout);

    void OnUpdate();

    int GetMeasureCount() const { return m_MeasureWidths.size(); }
    float GetMeasureWidth(int measureIndex) const;

    /**
     * Calculates the x position of the given measure relative to the system.
     *
     * @param measureIndex The index of the measure.
     * @return The x position of the measure relative to the system.
     */
    float GetMeasurePositionX(int measureIndex) const;

    float GetMeasurePositionY(int measureIndex) const; // for use with a non horizontal mode (like a vertical mode)
    float GetMeasureBeatPosition(int measureIndex) const;

    /**
     * Gets the width of the song (for Horizontal mode).
     *
     * @return The width of the song.
     */
    float GetSongWidth() const;

    /**
     * Converts a beatPosition into a normal x position.
     *
     * @param beatPositionInSong the beat position to be converted
     * @param currentMeasureIndex the index of the measure that the beat position is at
     * @return the x position in tenths
     */
    float GetPositionXInSong(float beatPositionInSong, int currentMeasureIndex) const;

    /**
     * Converts a beatPosition into a normal x position relative to the left-hand side of the measure.
     *
     * @param beatPositionInSong the beat position to be converted
     * @param currentMeasureIndex the index of the measure that the beat position is at
     * @return the x position in tenths
     */
    float GetPositionXInMeasure(float beatPositionInSong, int currentMeasureIndex) const;

    /**
     * Gets the y position of a system
     *
     * @param measureIndex the index of the first measure in the system
     * @return the y position of the system
     */
    float GetSystemPositionY(int measureIndex) const;

    /**
     * Calculates the height of a system
     *
     * @param systemIndex the index of the system
     * @return the height of the system
     */
    float GetSystemHeight(int systemIndex) const;

    BoundingBox GetSystemBoundingBox(int systemIndex) const;

    void CreatePageBreak(int measureIndex) const;
    void RemovePageBreak(int measureIndex) const;

    /**
     * Finds the index of the system that the given measure appears on.
     *
     * @param measureIndex the index of the measure
     * @return the index of the system
     */
    int GetSystemIndex(int measureIndex) const;

    /**
     * Finds the index of the page that the given measure appears on.
     *
     * @param measureIndex the index of the measure
     * @return the index of the page
     */
    int GetPageIndex(int measureIndex) const;

    /**
     * Finds the index of the first measure that appears on the given page index
     *
     * @param pageIndex the index of the page
     * @return the index of the measure
     */
    int GetFirstMeasureOnPage(int pageIndex) const;
    int GetFirstMeasureInSystem(int systemIndex) const;

    /**
     * Calculates the number of pages needed to render in paged mode.
     *
     * @return the number of pages
     */
    int GetNumPages() const;

    /**
     * Finds the minimum amount of space needed in front of the note.
     *
     * @param note A pointer to the note.
     * @return The amount of space in front of the note.
     */
    float GetNoteMinWidthInFront(const std::shared_ptr<Note>& note) const;

    /**
     * Finds the minimum amount of space needed behind the note.
     *
     * @param note A pointer to the note.
     * @return The amount of space behind the note.
     */
    float GetNoteMinWidthBehind(const std::shared_ptr<Note>& note) const;

    /**
     * Gets the measure at the given index (this will be the measure in the first instrument and staff).
     *
     * @param measureIndex the index of the measure
     * @return a pointer to the measure
     */
    std::shared_ptr<Measure> GetMeasure(int measureIndex) const;

    /**
     * Finds whether the measure at the given index is at the start of the system (i.e. it starts a new system).
     *
     * @param measureIndex the index of the measure.
     * @return weather the measure starts a new system.
     */
    bool DoesMeasureStartNewSystem(int measureIndex) const;

    /**
     * Finds whether the measure at the given index is at the start of the page (i.e. it starts a new page).
     *
     * @param measureIndex the index of the measure.
     * @return weather the measure starts a new page.
     */
    bool DoesMeasureStartNewPage(int measureIndex) const;

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
    float GetInstrumentPositionY(int measureIndex, int instrumentIndex) const;

    /**
     * Calculates the positions for every system relative to the page.
     *
     * @return A list of the positions for every system relative to the page.
     */
    std::vector<Vec2<float>> GetSystemPositions() const;

    /**
     * Gets the number of systems in the song.
     *
     * @return The number of systems.
     */
    int GetSystemCount() const { return systems.size(); }

    /**
     * Gets the measure at the specified point.
     *
     * @param point The point (x,y position).
     * @return A pointer to the Measure that collides with that point,
     * returns nullptr if the point doesn't collide with any Measures.
     */
    std::shared_ptr<Measure> GetMeasureAtPoint(Vec2<float> point, const std::vector<Vec2<float>>& systemPositions) const;

    void ResolveCollisions();

    void CalculateSystemPositionsAndPageBreaks();
private:
    struct TimeSpacePoint
    {
        float beatPositionInSong = 0.0f;
        float position = 0.0f;
        int measureIndex = 0;
    };

    void AddTimeSpacePoint(TimeSpacePoint point);

    void ResolveCollisionsWith(BoundingBox& box, int pageIndex);

    void ResolveCollisionWith(const std::shared_ptr<VisibleElement>& element, BoundingBox& box);
    void ResolveCollisionWith(VisibleElement* element, BoundingBox& box);

public:

    MusicDisplayConstants displayConstants = MusicDisplayConstants();

    Settings settings;

    std::string musicXMLVersion = "1.0";

    SongData songData = SongData();

    //std::vector<System::SystemLayout> systemLayouts;

    std::vector<std::shared_ptr<Instrument>> instruments;
    std::vector<std::shared_ptr<System>> systems;

    std::vector<Credit> credits;

    std::vector<Page> pages;

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
    int softwareMajorVersion = 1;
    std::string softwareName = "";

    Date encodingDate = Date();

    float totalBeatWidth = 0.0f;

    std::vector<std::shared_ptr<EndingGroup>> endingGroups;

    std::vector<BoundingBox> systemBoundingBoxes;

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