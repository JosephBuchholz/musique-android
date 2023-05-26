/**
 * This file contains the definition of the `Note` class.
 */

#ifndef MUSIQUE_NOTE_H
#define MUSIQUE_NOTE_H

class Note;

#include <vector>
#include "../Duration.h"
#include "../Pitch.h"
#include "TABSlur.h"
#include "NoteStem.h"
#include "NoteBeamData.h"
#include "Accidental.h"
#include "Lyric.h"
#include "../../MusicDisplayConstants.h"
#include "AugmentationDot.h"
#include "Beam.h"
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

/**
 * Class that represents a note, whether it is TAB or not.
 */
class Note : public VisibleElement {
    friend class Song;
    friend class MusicXMLParser;
    friend class NoteElementParser;
    friend class NoteChord;

public:
    ~Note() {}

    void Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float notePositionRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float measureWidth, int measureNumber, float ls, Vec2<float> mainPosition, int noteIndex, bool isLastMeasureInSystem, Vec2<float> offset = { 0.0f, 0.0f }) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

    static bool IsNoteIsHigher(Note* note1, Note* note2);

    void OnPlay();
    void OnStop();

    float GetMinWidth();

    void CalculateDurationTypeFromString(const std::string& s);

    Vec2<float> GetCenterPosition(const MusicDisplayConstants& displayConstants) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param displayConstants The display constants.
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const MusicDisplayConstants& displayConstants, const Vec2<float> &parentPosition);

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines);

private:
    void RenderRest(RenderData& renderData, float measurePositionX, int lines, float ls, float offsetX, float offsetY) const;
    void RenderTabNote(RenderData& renderData, TablatureDisplayType tabDisplayType, float measurePositionX, float measureWidth, int lines, float ls, float offsetX, float offsetY) const;

    void RenderTie(RenderData& renderData, float noteCenterPositionX, float notePositionY, float measurePositionX, float measurePositionY, float measureWidth) const;

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
    std::shared_ptr<NoteStem> noteStem;
    std::shared_ptr<NoteFlag> noteFlag;

    std::vector<NoteBeamData> beamData;

    Accidental accidental = Accidental();

    std::vector<Lyric> lyrics;

    std::vector<std::shared_ptr<Articulation>> articulations;
    std::vector<std::shared_ptr<Technique>> techniques;
    std::vector<std::shared_ptr<Ornament>> ornaments;

    std::shared_ptr<Fermata> fermata;
    std::shared_ptr<GlissandoSlide> glissSlide;
    std::shared_ptr<TremoloSingle> tremoloSingle;

    std::shared_ptr<NoteTie> tie;

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

    // -- Positioning Attributes --

    Vec2<float> position = { 0.0f, 0.0f }; // relative to measure

    // the size of this note (as a percentage)
    float size = 1.0f;

protected:

    // -- Positioning Attributes From MusicXML --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif // MUSIQUE_NOTE_H