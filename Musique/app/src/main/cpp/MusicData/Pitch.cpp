#include "Pitch.h"

float Pitch::GetPitchPositionY(const Clef& clef) const
{
    float position = 0.0f;

    // transpose pitch
    // pitch.octave += transpose.octaveChange;

    int y = GetLetterNumber(DiatonicNoteToString(step)) + octave*7; // the y position of pitch of the note

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

DiatonicNote Pitch::GetStepValueFromNoteNumber(int noteNumber)
{
    noteNumber = noteNumber % 12;
    if (noteNumber < 0)
        noteNumber += 12;

    if (noteNumber >= 11) {
        return DiatonicNote::B;
    } else if (noteNumber >= 9) {
        return DiatonicNote::A;
    } else if (noteNumber >= 7) {
        return DiatonicNote::G;
    } else if (noteNumber >= 5) {
        return DiatonicNote::F;
    } else if (noteNumber >= 4) {
        return DiatonicNote::E;
    } else if (noteNumber >= 2) {
        return DiatonicNote::D;
    } else if (noteNumber >= 0) {
        return DiatonicNote::C;
    }
    return DiatonicNote::C;
}

int Pitch::GetNoteNumberFromStepValue(DiatonicNote stepValue)
{
    int num = 0;
    if (stepValue == DiatonicNote::C) {
        num = 0;
    } else if (stepValue == DiatonicNote::D) {
        num = 2;
    } else if (stepValue == DiatonicNote::E) {
        num = 4;
    } else if (stepValue == DiatonicNote::F) {
        num = 5;
    } else if (stepValue == DiatonicNote::G) {
        num = 7;
    } else if (stepValue == DiatonicNote::A) {
        num = 9;
    } else if (stepValue == DiatonicNote::B) {
        num = 11;
    }

    return num;
}

int Pitch::GetPitchValue() const
{
    return GetNoteNumberFromStepValue(step) + (octave * 12) + alter;
}

void Pitch::SetPitchValue(int value)
{
    octave = value / 12;
    value -= octave * 12;
    step = GetStepValueFromNoteNumber(value);
    int stepNoteNumber = GetNoteNumberFromStepValue(step);
    alter = value - stepNoteNumber;
}
void Pitch::SetPitchValue(int value, float alter)
{
    // B# -> F## | F## -> B# | Cb -> Gb |
    this->alter = alter;

    octave = value / 12;
    value -= octave * 12;
    step = GetStepValueFromNoteNumber(value);
    int stepNoteNumber = GetNoteNumberFromStepValue(step);
    this->alter = value - stepNoteNumber;
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

void Pitch::OnTranspose(const TranspositionRequest& transposeRequest, const MusicalKey& currentKey)
{
    int interval = transposeRequest.GetInterval(); // interval between the keys
    int diatonicInterval = transposeRequest.GetDiatonicInterval(); // diatonic interval between the keys

    MusicalKey newKey = currentKey;
    if (currentKey.root == transposeRequest.originalKey.root)
        newKey = transposeRequest.key;
    else
        newKey.AddInterval(interval, diatonicInterval);

    diatonicInterval = transposeRequest.GetDiatonicInterval(newKey, currentKey);

    octave = ((octave * 7) + int(step) + diatonicInterval) / 7; // calculate new octave

    DiatonicNote originalStep = step; // save step for later calculation

    // calculate new step
    int newStep = ((int)step + diatonicInterval) % 7;
    // make sure it is in [0,7)
    if (newStep < 0)
        newStep += 7;
    step = DiatonicNote(newStep);

    // calculate new alter
    float alterFromKey = currentKey.GetAlterFromDiatonicNote(originalStep);
    float otherAlter = alter - alterFromKey;
    alter = otherAlter + newKey.GetAlterFromDiatonicNote(step);

    //LOGW_TAG("Pitch::OnTranspose", "interval: %d, diatonicInterval: %d, stepNumber: %d, value: %f, original: stepV %d, step %s, alter %f, octave %d || transposed: stepV %d, step %s, alter %f, octave %d || alterFromKey: %f, otherAlter: %f, transposedStep: %s"
    //         , interval, diatonicInterval, stepNumber, value, oStep, DiatonicNoteToString(oStep).c_str(), oAlter, oOctave, step, DiatonicNoteToString(step).c_str(), alter, octave, alterFromKey, otherAlter, DiatonicNoteToString(transposedStep).c_str());
}