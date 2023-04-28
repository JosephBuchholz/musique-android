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
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

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

    float defX = 0.0f;
    float defY = 0.0f;

    float relX = 0.0f;
    float relY = 0.0f;
};

#endif // MUSIQUE_NOTE_H