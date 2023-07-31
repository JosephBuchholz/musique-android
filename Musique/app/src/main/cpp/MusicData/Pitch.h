#ifndef MUSIQUE_PITCH_H
#define MUSIQUE_PITCH_H

#include <string>

#include "Measures/Clef.h"

struct Pitch
{
    Pitch() {}
    Pitch(std::string step, int octave, float alter)
        : step(step), octave(octave), alter(alter) {}

    float GetPitchPositionY(const Clef& clef) const;
    static float GetPitchPositionY(const Pitch& pitch, const Clef& clef);

    static std::string GetStepValueFromNoteNumber(int noteNumber);
    static int GetNoteNumberFromStepValue(std::string stepValue);

    int GetPitchValue() const;
    void SetPitchValue(int value);

    static int GetLetterNumber(const std::string& s);

    // for debug
    std::string GetPrintableString() const { return "Pitch: step: " + step + ", octave: " + ToString(octave) + ", alter: " + ToString(alter) + ", value: " + ToString(GetPitchValue()); }

public:
    std::string step = "C";
    int octave = 4;
    float alter = 0.0f;
    float freq = 0.0f;

public:

    bool operator==(const Pitch& rhs) const
    {
        return (this->GetPitchValue() == rhs.GetPitchValue());
    }

    bool operator<(const Pitch& rhs) const
    {
        return (this->GetPitchValue() < rhs.GetPitchValue());
    }

    bool operator>(const Pitch& rhs) const
    {
        return (this->GetPitchValue() > rhs.GetPitchValue());
    }

    bool operator<=(const Pitch& rhs) const
    {
        return (this->GetPitchValue() <= rhs.GetPitchValue());
    }

    bool operator>=(const Pitch& rhs) const
    {
        return (this->GetPitchValue() >= rhs.GetPitchValue());
    }
};

#endif // MUSIQUE_PITCH_H