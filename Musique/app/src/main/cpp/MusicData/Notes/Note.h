/**
 * This file contains the definition of the `Note` class.
 */

#ifndef MUSIQUE_NOTE_H
#define MUSIQUE_NOTE_H

class Note;

#include <vector>
#include "../Duration.h"
#include "../Pitch.h"
#include "../Transpose.h"
#include "TABSlur.h"
#include "NoteStem.h"
#include "Beams/NoteBeamData.h"
#include "Accidental.h"
#include "Lyric.h"
#include "../../MusicDisplayConstants.h"
#include "AugmentationDot.h"
#include "Beams/Beam.h"
#include "Tie.h"
#include "../BaseElements/VisibleElement.h"
#include "Articulations/Articulation.h"
#include "Techniques/Technique.h"
#include "NoteHead.h"
#include "Fermata.h"
#include "Ornaments/Ornaments.h"
#include "GlissandoSlide.h"
#include "TremoloSingle.h"
#include "NoteFlag.h"

#include "../../MusicPlayers/Player.h"

/**
 * Class that represents a note, whether it is TAB or not.
 */
class Note : public VisibleElement
{
    friend class Song;
    friend class MusicXMLParser;
    friend class NoteElementParser;
    friend class NoteChord;

public:
    ~Note() {}

    /**
     * Renders this note.
     *
     * @param renderData The RenderData to render to.
     * @param tabDisplayType
     * @param notePositionRelativeToMeasures
     * @param lines The number of staff lines.
     * @param measurePosition The position of the current measure.
     * @param nextMeasurePositionX The x position of the next measure.
     * @param ls Staff line spacing.
     */
    void Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float notePositionRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float ls) const;

    /**
     * Same as the 'Render' function except for debug rendering.
     */
    void RenderDebug(RenderData& renderData, TablatureDisplayType tabDisplayType, float notePositionRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float ls) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

    static bool IsNoteIsHigher(Note* note1, Note* note2);

    float GetMinWidth() const;

    void CalculateDurationTypeFromString(const std::string& s);

    Vec2<float> GetCenterPosition(const MusicDisplayConstants& displayConstants) const;

    /**
     * Get the bounding box containing just this note (notehead, stem).
     *
     * @param displayConstants Display constants.
     * @return The bounding box.
     */
    BoundingBox GetBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const;

    /**
     * Get the bounding box containing everything including this note,
     * fermatas, and anything else that this note contains.
     *
     * @param displayConstants Display constants.
     * @return The bounding box.
     */
    BoundingBox GetTotalBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param displayConstants The display constants.
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition);

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines);
    void UpdateTieAndGlissSlide(bool measureStartsNewSystem);

    void OnTranspose(const TranspositionRequest& transposeRequest, const MusicalKey& currentKey);

    /* ----- Sound Related Functions ----- */

    void InitSound(const std::shared_ptr<Note>& previousNote);

    void OnPlay(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity);
    void OnStop(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity);
    void OnUpdate(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity, float beatPositionRelativeToNote, float previousBeatPositionRelativeToNote);

    static SMuFLID GetRestSMuFLID(NoteValue type);

private:
    void PlayPitch(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity);
    void StopPitch(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity);

private:
    void RenderRest(RenderData& renderData, float measurePositionX, int lines, float ls, float offsetX, float offsetY) const;

    void RenderAugmentationDots(RenderData& renderData, float notePositionX, float notePositionY) const;

public:

    NoteSize noteSize = NoteSize::Normal;

    bool isPlaying = false;
    NoteType type = NoteType::Standard;
    NoteValue durationType = NoteValue::Whole;
    Duration duration = Duration();
    Pitch pitch = Pitch();
    float beatPosition = 0.0f; // the position of the note in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the note in the song(not counting repeats) in beats(quarter notes)

    int measureIndex = 0; // the measure that this note is in

    //std::vector<Slur> slurs;

    std::vector<AugmentationDot> dots;

    NoteHead noteHead = NoteHead();
    NoteStem noteStem = NoteStem();
    NoteFlag noteFlag = NoteFlag();

    std::vector<NoteBeamData> beamData;

    std::shared_ptr<Accidental> accidental;

    std::vector<Lyric> lyrics;

    std::vector<std::shared_ptr<Articulation>> articulations;
    std::vector<std::shared_ptr<Technique>> techniques;
    std::vector<std::shared_ptr<Ornament>> ornaments;

    std::shared_ptr<Fermata> fermata;
    std::vector<std::shared_ptr<GlissandoSlide>> glissSlides; // including incoming and outgoing slides
    std::shared_ptr<TremoloSingle> tremoloSingle;

    std::shared_ptr<NoteTie> tie; // TODO: should be vector (to include incoming and outgoing ties)

    int staff = 1;
    int voice = 1;

    bool isRest = false;
    bool isFullMeasureRest = false;

    bool isChord = false; // weather or not the note is played at the same time as the previous note

    // grace note attributes
    bool isGraceNote = false;
    bool hasSlash = false;

    // -- Tab Only --

    int string = 0;
    int fret = 0;

    //std::vector<TABSlur> tabSlurs; // hammer ons and pull offs

    /* ----- Sound Related ----- */

    float soundBeatPosition = 0.0f; // the beat position of the actual sounding of the note (in the measure)
    float soundDuration = 0.0f; // the sounding duration of the note

protected:

    // -- Positioning Attributes From MusicXML --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif // MUSIQUE_NOTE_H