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
        positionX = defX + relX;
        positionY = -defY + -relY;
    }
    else if (type == Note::NoteType::Tab) // is a tab note
    {
        positionX = defX + relX;
        positionY = (displayConstants.tabLineSpacing * float(string - 1));

        if (!isChord)
        {
            // note: all stem positions are relative to the note

            float stemLength = displayConstants.tabLineSpacing * 2.5f;

            if (durationType == NoteDurationType::Half) // shorter stem for half notes
            {
                stemLength *= 2.0f/3.0f;
            }

            noteStem.stemPositionX = 0.0f;

            if (noteStem.stemType == NoteStem::StemType::Up)
            {
                noteStem.stemStartY = -(displayConstants.tabLineSpacing * 0.75f);
                noteStem.stemEndY = noteStem.stemStartY - stemLength;
            }
            else if (noteStem.stemType == NoteStem::StemType::Down)
            {
                noteStem.stemStartY = displayConstants.tabLineSpacing * 0.75f;
                noteStem.stemEndY = noteStem.stemStartY + stemLength;
            }
        }
    }
    else // is a standard note
    {
        positionX = defX + relX;
        positionY = -defY + -relY;

        float noteWidth = 11.3f;
        float stemPositionX = 0.0f;

        float notePositionX = 0.0f;
        float notePositionY = 0.0f;

        float stemStokeWidth = 0.8333f;

        float stemLength = 30.0f;
        if (noteStem.stemType == NoteStem::StemType::Up) {
            noteStem.stemPositionX = notePositionX + noteWidth - stemStokeWidth / 2.0f;

            noteStem.stemStartY = notePositionY;
            noteStem.stemEndY = notePositionY - stemLength;
        } else if (noteStem.stemType == NoteStem::StemType::Down) {
            noteStem.stemPositionX = notePositionX + stemStokeWidth / 2.0f;

            noteStem.stemStartY = notePositionY;
            noteStem.stemEndY = notePositionY + stemLength;
        }
    }

    for (auto& dot : dots)
    {
        dot.CalculatePositionAsPaged(displayConstants, ((positionY / displayConstants.lineSpacing) - floor(positionY / displayConstants.lineSpacing)) == 0.0f, type == NoteType::Tab);
    }
}