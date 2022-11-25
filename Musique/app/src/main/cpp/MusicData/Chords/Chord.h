#pragma once

#include <vector>

#include "../TextualElement.h"
#include "../String.h"
#include "../Pitch.h"
#include "ChordDegree.h"
#include "../Duration.h"

class Chord : public TextualElement {

public:
    Chord() {}

    void CalculateChordName();

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