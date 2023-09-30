/**
 * This file defines classes for pitch.
 */

#ifndef MUSIQUE_PITCH_H
#define MUSIQUE_PITCH_H

#include <string>

#include "Measures/Clef.h"
#include "../TranspositionRequest.h"
#include "Types.h"

/**
 * Represents a basic pitch using a "pitch value" and frequency.
 */
struct BasicPitch
{
    float pitchValue = 0; // the value of the pitch relative to 12-TET system (C4 -> 12 * 4 -> 48), use fractions for other pitches
    float freq = 0.0f; // the actual frequency of the pitch in Hz
};

/**
 * Represents a pitch of a note on a staff. (allows for enharmonic equivalence)
 */
class Pitch : public BasicPitch
{
public:
    Pitch() {}
    Pitch(DiatonicNote step, int octave, float alter)
        : step(step), octave(octave), alter(alter) {}

    float GetPitchPositionY(const Clef& clef) const;
    static float GetPitchPositionY(const Pitch& pitch, const Clef& clef);

    static DiatonicNote GetStepValueFromNoteNumber(int noteNumber);
    static int GetNoteNumberFromStepValue(DiatonicNote stepValue);

    int GetPitchValue() const;
    void SetPitchValue(int value);
    void SetPitchValue(int value, float alter);

    static int GetLetterNumber(const std::string& s);

    void OnTranspose(const TranspositionRequest& transposeRequest, const MusicalKey& currentKey);

    // for debug
    std::string GetPrintableString() const { return "Pitch: step: " + DiatonicNoteToString(step) + ", octave: " + ToString(octave) + ", alter: " + ToString(alter) + ", value: " + ToString(GetPitchValue()); }

public:
    DiatonicNote step = DiatonicNote::C;
    int octave = 4;
    float alter = 0.0f;

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