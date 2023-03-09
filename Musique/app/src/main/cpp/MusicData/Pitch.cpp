#include "Pitch.h"

float Pitch::GetPitchPositionY(const Clef& clef) const
{
    float position = 0.0f;

    // transpose pitch
    // pitch.octave += transpose.octaveChange;

    int y = GetLetterNumber(step) + octave*7; // the y position of pitch of the note

    int line = 5 - clef.line; // getting line starting from the top instead of the bottom
    if (clef.sign == "G" || clef.sign == "F" || clef.sign == "C") {
        int clefOctave = 4; // the octave of the clef sign
        if (clef.sign == "F")
            clefOctave = 3;
        int clefY = GetLetterNumber(clef.sign) + clefOctave*7; // the y position of pitch of the clef

        position = (float)line - (y - clefY) * 0.5f; // the y position of the note on the staff
    }

    return position;
}

float Pitch::GetPitchPositionY(const Pitch& pitch, const Clef& clef)
{
    return pitch.GetPitchPositionY(clef);
}

std::string Pitch::GetStepValueFromNoteNumber(int noteNumber)
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

int Pitch::GetPitchValue() const
{
    return GetLetterNumber(step) + (octave * 12) + alter;
}

int Pitch::GetLetterNumber(const std::string& s)
{
    int num = 0;
    if (s == "C") {
        num = 0;
    } else if (s == "D") {
        num = 1;
    } else if (s == "E") {
        num = 2;
    } else if (s == "F") {
        num = 3;
    } else if (s == "G") {
        num = 4;
    } else if (s == "A") {
        num = 5;
    } else if (s == "B") {
        num = 6;
    }
    return num;
}