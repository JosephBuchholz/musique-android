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
#include "../../Settings.h"

class Chord : public TextualElement
{
    friend class MusicXMLParser;

public:
    Chord() {}

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition) const;
    void RenderDebug(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition) const;

    BoundingBox GetBoundingBoxRelativeToParent() const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float> &parentPosition);

    void CalculateChordName(const Settings& settings);

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY);

    void OnTranspose(const TranspositionRequest& transposeRequest, const MusicalKey& currentKey);

private:

    void AddCharsToString(std::vector<uint16_t>& dest, const std::vector<uint16_t>& charsToAdd);
    void AddCharsToString(std::vector<uint16_t>& dest, const std::string& charsToAdd);

    std::vector<uint16_t> GetHarmonyStringWithSymbols() const;
    std::vector<uint16_t> GetHarmonyStringWithoutSymbols() const;

    static std::vector<uint16_t> GetTextFromPitch(const Pitch& pitch);

    static SMuFLID GetChordSymbolAccidentalSMuFLID(int alter);

public:

    int staff = 1;

    bool isPlaying = false;
    Duration duration = Duration();
    float beatPosition = 0.0f; // the position of the note in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the note in the song(not counting repeats) in beats(quarter notes)

    std::vector<uint16_t> chordNameString;
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