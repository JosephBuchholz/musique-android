#ifndef MUSIQUE_CHORD_H
#define MUSIQUE_CHORD_H

#include <vector>

#include "../BaseElements/TextualElement.h"
#include "../String.h"
#include "../Pitch.h"
#include "ChordDegree.h"
#include "../Duration.h"
#include "ChordDiagram.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/RenderData.h"

class Chord : public TextualElement
{
    friend class Song;
    friend class MusicXMLParser;

public:
    Chord() {}

    void Render(RenderData& renderData, float measurePositionX, float measurePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float> &parentPosition);

    void CalculateChordName();

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY);

public:

    int staff = 1;

    bool isPlaying = false;
    Duration duration = Duration();
    float beatPosition = 0.0f; // the position of the note in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the note in the song(not counting repeats) in beats(quarter notes)

    String chordName = String();
    Pitch rootPitch;

    bool hasBassNote = false;
    Pitch bassPitch;
    String bassSeparator = String("/");
    enum class BassPos {
        None = 0, Horizontal, Vertical, Diagonal
    } bassPos = BassPos::Horizontal;

    std::vector<ChordDegree> degrees;

    bool brackets = false;
    bool parentheses = false;
    bool stackDegrees = false;
    bool useSymbols = false;

    std::string harmonyTypeText = "";

    std::shared_ptr<ChordDiagram> chordDiagram;

    enum class HarmonyType {
        None = 0,
        NoHarmony,
        Other,
        Augmented,
        AugmentedSeventh,
        Diminished,
        DiminishedSeventh,
        Dominant,
        Dominant11th,
        Dominant13th,
        DominantNinth,
        French,
        German,
        HalfDiminished,
        Italian,
        Major,
        Major11th,
        Major13th,
        MajorMinor,
        MajorNinth,
        MajorSeventh,
        MajorSixth,
        Minor,
        Minor11th,
        Minor13th,
        MinorNinth,
        MinorSeventh,
        MinorSixth,
        Neapolitan,
        Pedal,
        Power,
        SuspendedFourth,
        SuspendedSecond,
        Tristan
    } harmonyType = HarmonyType::None;
};

#endif // MUSIQUE_CHORD_H