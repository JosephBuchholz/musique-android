/**
 * This file contains the definition of the `Note` class.
 */

#ifndef MUSIQUE_NOTE_H
#define MUSIQUE_NOTE_H

#include <vector>
#include "../Duration.h"
#include "../Pitch.h"
#include "TABSlur.h"
#include "Slur.h"
#include "NoteStem.h"
#include "NoteBeamData.h"
#include "Accidental.h"
#include "Lyric.h"
#include "../../MusicDisplayConstants.h"
#include "AugmentationDot.h"
#include "Beam.h"
#include "Tie.h"
#include "../VisibleElement.h"
#include "Articulations/Articulation.h"
#include "Techniques/Technique.h"

/**
 * Class that represents a note, whether it is TAB or not.
 */
class Note : public VisibleElement {
    friend class Song;
    friend class MusicXMLParser;
    friend class NoteElementParser;

public:
    ~Note() {}

    void Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float notePositionRelativeToMeasure, int lines, Vec2<float> measurePosition, float measureWidth, int measureNumber, float ls, Vec2<float> mainPosition, int noteIndex, Vec2<float> offset = { 0.0f, 0.0f }) const;

    static bool IsNoteIsHigher(Note* note1, Note* note2);

    void OnPlay();
    void OnStop();

    float GetMinWidth();

    void CalculateDurationTypeFromString(const std::string& s);

    float GetCenterPositionX() const;

    float GetNoteHeadWidth() const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float> &parentPosition);

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines);

private:
    void RenderRest(RenderData& renderData, const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY) const;
    void RenderTabNote(RenderData& renderData, const Note* note, TablatureDisplayType tabDisplayType, float measurePositionX, float measureWidth, int lines, float ls, float offsetX, float offsetY) const;

    void RenderNoteFlag(RenderData& renderData, const Note* note, float notePositionX, float notePositionY) const;
    void RenderNoteStem(RenderData& renderData, const Note* note, float notePositionX, float notePositionY) const;

    void RenderTie(RenderData& renderData, const Note* note, float noteCenterPositionX, float notePositionY, float measurePositionX, float measurePositionY, float measureWidth) const;

    void RenderAugmentationDots(RenderData& renderData, float notePositionX, float notePositionY) const;

public:

    enum class NoteType {
        None = 0, Standard, Tab
    };

    bool isPlaying = false;
    NoteType type = NoteType::Standard;
    NoteValue durationType = NoteValue::Whole;
    Duration duration = Duration();
    Pitch pitch = Pitch();
    float beatPosition = 0.0f; // the position of the note in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the note in the song(not counting repeats) in beats(quarter notes)

    int measureIndex = 0; // the measure that this note is in

    std::vector<Slur> slurs;

    std::vector<AugmentationDot> dots;

    NoteStem noteStem = NoteStem();

    std::vector<NoteBeamData> beamData;

    Accidental accidental = Accidental();

    std::vector<Lyric> lyrics;

    std::vector<std::shared_ptr<Articulation>> articulations;
    std::vector<std::shared_ptr<Technique>> techniques;

    NoteTie tie = NoteTie();

    int staff = 1;
    int voice = 1;

    bool isRest = false;
    bool isFullMeasureRest = false;

    bool isChord = false; // weather or not the note is played at the same time as the previous note

    // -- Tab Only --

    int string = 0;
    int fret = 0;

    std::vector<TABSlur> tabSlurs; // hammer ons and pull offs

    // -- Positioning Attributes --

    float positionX = 0.0f; // relative to measure
    float positionY = 0.0f; // relative to measure

protected:

    // -- Positioning Attributes From MusicXML --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif // MUSIQUE_NOTE_H