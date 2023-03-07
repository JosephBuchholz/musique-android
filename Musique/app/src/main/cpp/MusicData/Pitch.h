#ifndef MUSIQUE_PITCH_H
#define MUSIQUE_PITCH_H

#include <string>

struct Pitch {

    Pitch() {}
    Pitch(std::string step, int octave, float alter)
        : step(step), octave(octave), alter(alter) {}

    static std::string GetStepValueFromNoteNumber(int noteNumber)
    {
        noteNumber = noteNumber % 12;
        if (noteNumber >= 11) {
            return "B";
        } else if (noteNumber >= 9) {
            return "A";
        } else if (noteNumber >= 7) {
            return "G";
        } else if (noteNumber >= 5) {
            return "F";
        } else if (noteNumber >= 4) {
            return "E";
        } else if (noteNumber >= 2) {
            return "D";
        } else if (noteNumber >= 0) {
            return "C";
        }
        return "";
    }

    int GetPitchValue()
    {
        int num = 0;
        if (step == "C") {
            num = 0;
        } else if (step == "D") {
            num = 1;
        } else if (step == "E") {
            num = 2;
        } else if (step == "F") {
            num = 3;
        } else if (step == "G") {
            num = 4;
        } else if (step == "A") {
            num = 5;
        } else if (step == "B") {
            num = 6;
        }
        return num + (octave * 12) + alter;
    }

    std::string step = "C";
    int octave = 4;
    float alter = 0.0f;
    float freq = 0.0f;
};

#endif // MUSIQUE_PITCH_H