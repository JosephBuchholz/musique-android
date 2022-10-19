#pragma once
#include <vector>
#include "Duration.h"
#include "Pitch.h"
#include "TABSlur.h"
#include "Slur.h"
#include "NoteStem.h"
#include "Beam.h"
#include "Accidental.h"
#include "Lyric.h"

class Note {

public:
    ~Note() {}

    void OnPlay() {
        isPlaying = true;
    }

    void OnStop() {
        isPlaying = false;
    }

    float GetMinWidth() {
        float width = 0.0f;
        width += duration.duration * 5.0f * 10.0f; // should do a bit more calculations here
        if (!lyrics.empty())
            width += 20.0f;
        return width;
    }

    void CalculateDurationTypeFromString(const std::string& s) {
        if (s == "whole") {
            durationType = NoteDurationType::Whole;
        } else if (s == "half") {
            durationType = NoteDurationType::Half;
        } else if (s == "quarter") {
            durationType = NoteDurationType::Quarter;
        } else if (s == "eighth") {
            durationType = NoteDurationType::Eighth;
        } else if (s == "16th") {
            durationType = NoteDurationType::Sixteenth;
        } else if (s == "32nd") {
            durationType = NoteDurationType::ThirtySecond;
        } else {
            durationType = NoteDurationType::None;
        }
    }

    enum class NoteType {
        None = 0, Standard, Tab
    };

    enum class NoteDurationType {
        None = 0, Whole, Half, Quarter, Eighth, Sixteenth, ThirtySecond
    };

    bool isPlaying = false;
    NoteType type = NoteType::Standard;
    NoteDurationType durationType = NoteDurationType::Whole;
    Duration duration = Duration();
    Pitch pitch = Pitch();
    float beatPosition = 0.0f; // the position of the note in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the note in the song(not counting repeats) in beats(quarter notes)

    int measureIndex = 0; // the measure that this note is in

    std::vector<Slur> slurs;

    NoteStem noteStem = NoteStem();

    Beam beam = Beam();

    Accidental accidental = Accidental();

    std::vector<Lyric> lyrics;

    int staff = 1;
    int voice = 1;

    bool isRest = false;
    bool isFullMeasureRest = false;

    bool isChord = false; // weather or not the note is played at the same time as the previous note


    // -- Tab Only --
    int string = 0;
    int fret = 0;

    std::vector<TABSlur> tabSlurs; // hammer ons and pull offs
};