#ifndef MUSIQUE_PITCH_H
#define MUSIQUE_PITCH_H

#include <string>

#include "Measures/Clef.h"

struct Pitch {

    Pitch() {}
    Pitch(std::string step, int octave, float alter)
        : step(step), octave(octave), alter(alter) {}

    float GetPitchPositionY(const Clef& clef) const;
    static float GetPitchPositionY(const Pitch& pitch, const Clef& clef);

    static std::string GetStepValueFromNoteNumber(int noteNumber);

    int GetPitchValue() const;

    static int GetLetterNumber(const std::string& s);

    std::string step = "C";
    int octave = 4;
    float alter = 0.0f;
    float freq = 0.0f;
};

#endif // MUSIQUE_PITCH_H