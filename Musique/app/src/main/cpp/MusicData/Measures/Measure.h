/**
 * This file contains the definition for the `Measure` class which represents a musical measure.
 */

#ifndef MUSIQUE_MEASURE_H
#define MUSIQUE_MEASURE_H

class Measure;
class BeamGroup;
class Tuplet;
class Arpeggio;
class Slur;

#include "../Notes/Note.h"
#include "TimeSignature.h"
#include "KeySignature.h"
#include "Clef.h"
#include "../Transpose.h"
#include "../Directions/Direction.h"
#include "../SoundEvent.h"
#include "../Chords/Chord.h"
#include "Barline.h"
#include "../Notes/Beams/BeamGroup.h"
#include "../BaseElements/VisibleElement.h"
#include "Tuplet.h"
#include "MeasureNumber.h"
#include "Arpeggio.h"
#include "MeasureRepeat.h"
#include "../Notes/Slur.h"
#include "../Notes/NoteChord.h"
#include "MultiMeasureRestSymbol.h"

#include "../../MusicPlayers/Player.h"
#include "../../TranspositionRequest.h"

#include <vector>

/**
 * A class that represents a measure.
 */
class Measure : public VisibleElement
{
    friend class Staff;
    friend class Song;
    friend class MusicXMLParser;

public:

    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int staffLineCount) const;

    static float GetMiddleHeight(int staffLineCount, float lineSpacing);
    float GetAboveHeight(const MusicDisplayConstants& displayConstants, int staffLineCount) const;
    float GetBelowHeight(int staffLineCount, float lineSpacing);

    float GetTotalHeight(const MusicDisplayConstants& displayConstants, int staffLineCount) const;

    float GetBeginningWidth(const std::shared_ptr<System>& system) const;

    float CalculateMinWidth(float notesWidth) const;

    float GetNotePositionInMeasure(float width, int noteIndex) const;

    float GetKeySignaturePositionInMeasure(const std::shared_ptr<System>& system, float clefPositionX) const;
    float GetTimeSignaturePositionInMeasure(const std::shared_ptr<System>& system, float keySignaturePositionX) const;
    float GetClefPositionInMeasure(const std::shared_ptr<System>& system) const;

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition, float nextMeasurePositionX, const std::shared_ptr<System>& system, int staffLineCount, float staffLineSpacing, bool isTopMeasureLine, bool isLastMeasureInSystem, TablatureDisplayType tablatureDisplayType, bool isPartOfEnding) const;

    /**
     * Same as the 'Render' function except for debug rendering.
     */
    void RenderDebug(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition, float nextMeasurePositionX, const std::shared_ptr<System>& system, int staffLineCount, float staffLineSpacing, bool isTopMeasureLine, bool isLastMeasureInSystem, TablatureDisplayType tablatureDisplayType) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

    /**
     * The x position at which a repeat barline at the beginning of the measure would be.
     *
     * @return the x position relative to the start of the measure.
     */
    float GetRepeatBarlinePositionX(const std::shared_ptr<System>& system) const;

    float GetPitchYPosition(const Pitch& pitch) const;
    float CalculateNoteYPositionRelativeToMeasure(int noteIndex) const;
    float CalculateNoteYPositionRelativeToMeasure(const std::shared_ptr<Note>& note) const;
    static int GetLetterNumber(const std::string& s) ;

    float GetPlayLinePositionInMeasure(float playLineBeatPositionInMeasure, float width) {
        return (playLineBeatPositionInMeasure / duration.duration) * width; // <-- need to be better
    }

    float GetDuration() const { return duration.duration; }

    void CalculateDuration() {
        duration.duration = timeSignature.notes * (4.0f / timeSignature.noteType); // duration in beats(quarter notes)
    }

    void UpdateBoundingBoxes(const MusicDisplayConstants& displayConstants, const Vec2<float>& measurePosition, float measureHeight);

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants, const Settings& settings, const std::shared_ptr<System>& system, int staffLines);

    bool IsRepeatBackward() const;
    bool IsRepeatForward() const;

    int GetRepeatCount() const;

    bool IsOnBeatEighthNote(const std::shared_ptr<Note>& note) const;
    bool IsOffBeatEighthNote(const std::shared_ptr<Note>& note) const;

    bool IsOnBeatSixteenthNote(const std::shared_ptr<Note>& note) const;
    bool IsOffBeatSixteenthNote(const std::shared_ptr<Note>& note) const;

    void InitBeatPosition(float measureBeatPosition);

    bool UsesOpenStrings() const;
    void OnTranspose(const TranspositionRequest& transposeRequest);

public:

    /* ----- Sound Related Functions ----- */

    void OnUpdate(const std::shared_ptr<Player>& player, bool isCurrentMeasure, int channel, float playLineBeatPosition, float previousPlayLineBeatPosition, float measureBeatPosition, float& currentTempo, SwingTempo& swingTempo, float& currentVelocity, uint32_t& capo);

private:
    float MeausreClefWidth() const;
    float MeausreKeySignatureWidth() const;
    float MeausreTimeSignatureWidth() const;

    void RenderBarlines(RenderData& renderData, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing, bool isTopStaff, bool isPartOfEnding) const;
    void RenderMeasureBeginning(RenderData& renderData, Vec2<float> measurePosition, const std::shared_ptr<System>& system, int staffLineCount, float staffLineSpacing, bool isTopMeasureLine) const;
    void RenderStaffLines(RenderData& renderData, Vec2<float> measurePosition, int staffLineCount, float staffLineSpacing) const;

public:

    enum class MeasureType {
        None = 0, Standard, Tab
    };

    MeasureType type = MeasureType::Standard;

    TimeSignature timeSignature = TimeSignature();
    bool showTimeSignature = false;

    KeySignature keySignature = KeySignature(0);
    bool showKeySignature = false;

    Clef clef = Clef();

    int staff = 1; // the staff that this measure belongs to

    Transpose transpose = Transpose();

    MeasureNumber measureNumber; // the number that is displayed for this measure.
    int implicit = false; // If it is true then the measure number should never appear (this is used for measure pickups)

    int index = 0;

    float staffDistance = 0.0f;

    Duration duration = Duration(); // (hard coded) the duration of the measure
    float beatPosition = 0.0f; // the position in beats in the song

    std::vector<std::shared_ptr<Note>> notes; // the notes contained in this measure
    std::vector<std::shared_ptr<NoteChord>> noteChords; // the note chords contained in this measure

    std::vector<std::shared_ptr<SoundEvent>> soundEvents; // the sound events contained in this measure

    std::vector<Direction> directions; // the directions contained in this measure
    std::vector<Chord> chords; // the chords contained in this measure
    std::vector<Barline> barlines; // the barlines contained in this measure
    std::vector<BeamGroup> beams; // the beams (beam groups) contained in this measure
    std::vector<std::shared_ptr<Tuplet>> tuplets; // the tuplets that are contained in this measure
    std::vector<std::shared_ptr<Arpeggio>> arpeggios; // the arpeggios that are contained in this measure
    std::vector<std::shared_ptr<Slur>> slurs; // slurs that are contained in this measures

    int divisions = 1;

    float measureWidth = 0.0f;

    bool startsMultiMeasureRest = false;
    std::shared_ptr<MultiMeasureRestSymbol> multiMeasureRestSymbol; // only used when 'startsMultiMeasureRest' is true

    bool isPartOfMultiMeasureRest = false; // if this is true and 'startsMultiMeasureRest' is not, then this measure should not be rendered

    bool isMeasureRepeat = false; // whether a simile symbol is used for this measure
    std::shared_ptr<MeasureRepeat> measureRepeat; // a simile/measure repeat symbol (only used if 'isMeasureRepeat' is true)

protected:
    // -- Constants In MusicXML --

    // as stated in the file (these are in tenths)
    float defaultMeasureWidth = -1.0f; // negative means that it wasn't mentioned in the file

    float defStaffDistance = -1.0f; // if negative then use the default value in displayConstants

    bool startNewSystem = false;
    bool startNewPage = false;

    // -- Other --

    float nextBeatPosition = -1.0f; // -1.0f means it is not a valid value
};

#endif // MUSIQUE_MEASURE_H