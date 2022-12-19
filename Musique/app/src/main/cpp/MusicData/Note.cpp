#include "Note.h"

bool Note::IsNoteIsHigher(Note* note1, Note* note2)
{
    if (note1->pitch.GetPitchValue() > note2->pitch.GetPitchValue())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Note::OnPlay() {
    isPlaying = true;
}

void Note::OnStop() {
    isPlaying = false;
}

float Note::GetMinWidth() {
    float width = 0.0f;
    width += duration.duration * 5.0f * 10.0f; // should do a bit more calculations here
    if (!lyrics.empty())
        width += 20.0f;
    return width;
}

void Note::CalculateDurationTypeFromString(const std::string& s) {
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

void Note::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{
    if (isRest) // is a rest
    {
        positionX = defaultX;
        positionY = defaultY;
    }
    else if (type == Note::NoteType::Tab) // is a tab note
    {
        positionX = defaultX;
        positionY = (displayConstants.tabLineSpacing * float(string - 1));
    }
    else // is a standard note
    {
        positionX = defaultX;
        positionY = defaultY;
    }
}