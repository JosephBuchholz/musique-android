#pragma once
#include "Note.h"
#include "TimeSignature.h"
#include "KeySignature.h"
#include "Clef.h"
#include "Transpose.h"
#include "Direction.h"
#include "SoundEvent.h"
#include <vector>

class Measure {

public:

    float GetMiddleHeight(float staffLineCount, float lineSpacing);
    float GetAboveHeight(float staffLineCount, float lineSpacing);
    float GetBelowHeight(float staffLineCount, float lineSpacing);
    float GetTotalHeight(float staffLineCount, float lineSpacing);

    float GetBeginningWidth() const {
        float width = 20.0f;

        if (showKeySignature)
        {
            width += 20.0f;
            width += 20.0f;
        }

        if (showTimeSignature)
        {
            width += 16.0f;
            width += 20.0f;
        }

        if (showClef)
        {
            width += 17.0f;
            width += 20.0f;
        }
        return width;
    }

    float CalculateMinWidth(float notesWidth) const {
        float width = GetBeginningWidth();

        /*for (auto& note : notes) {
            if (!note->isChord) {
                width += note->GetMinWidth();
            }
        }*/
        width += notesWidth;

        width += 10.0f;

        if (width == 0.0f) {
            width = 1.0f;
        }

        return width;
    }

    // update to use measureWidth
    float GetNotePositionInMeasure(float measureWidth, int noteIndex) const {

        // if the note index is a chord it gets the root note of the chord
        while (true) {
            if (notes[noteIndex]->isChord && noteIndex != 0) {
                noteIndex -= 1;
                continue;
            }
            break;
        }

        float position = GetBeginningWidth();
        int i = 0;
        for (auto& note : notes) {

            if (i == noteIndex) {
                break;
            } else {
                if (!note->isChord) {
                    position += note->GetMinWidth();
                }
            }

            i++;
        }

        return position;
    }

    // update to use measureWidth
    float GetKeySignaturePositionInMeasure(float measureWidth) const {
        float position = GetClefPositionInMeasure(measureWidth) + 17.0f + 20.0f;

        return position;
    }

    // update to use measureWidth
    float GetTimeSignaturePositionInMeasure(float measureWidth) const {

        float position = GetKeySignaturePositionInMeasure(measureWidth) + 20.0f + 20.0f;

        return position;
    }

    // update to use measureWidth
    float GetClefPositionInMeasure(float measureWidth) const {
        float position = 10.0f;

        return position;
    }

    float GetNoteYPosition(int noteIndex) const {
        float position = 0.0f;

        Note* note = notes[noteIndex];

        int y = GetLetterNumber(note->pitch.step) + note->pitch.octave*7; // the y position of pitch of the note

        int line = 5 - clef.line; // getting line starting from the top instead of the bottom
        if (clef.sign == "G" || clef.sign == "F" || clef.sign == "C") {
            int octave = 4; // the octave of the clef sign
            int clefY = GetLetterNumber(clef.sign) + octave*7; // the y position of pitch of the clef

            position = (float)line - (y - clefY) * 0.5f; // the y position of the note on the staff
        }

        return position;
    }

    int GetLetterNumber(const std::string& s) const {
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

    float GetPlayLinePositionInMeasure(float playLineBeatPositionInMeasure, float measureWidth) {
        return (playLineBeatPositionInMeasure / duration.duration) * measureWidth; // <-- need to be better
    }

    float GetDuration() const { return duration.duration; }

    void CalculateDuration() {
        duration.duration = timeSignature.notes * (4.0f / timeSignature.noteType); // duration in beats(quarter notes)
    }

    /*void CopyData(Measure* measure) // copies all data from measure except any notes that don't equal this measure's 'staff' and the 'staff' variable itself
    {
        timeSignature = measure->timeSignature;
        showTimeSignature = measure->showTimeSignature;
        keySignature = measure->keySignature;
        showKeySignature = measure->showKeySignature;
        clef = measure->clef;
        showClef = measure->showClef;
        transpose = measure->transpose;
        duration = measure->duration;
        beatPosition = measure->beatPosition;
        divisions = measure->divisions;
        nextBeatPosition = measure->nextBeatPosition;

        for (Note* note : measure->notes) // only notes that equal this measure's staff
        {
            if (note->staff == staff)
            {
                notes.push_back(note);
            }
        }
    }*/

    TimeSignature timeSignature = TimeSignature();
    bool showTimeSignature = false;

    KeySignature keySignature = KeySignature(0);
    bool showKeySignature = false;

    Clef clef = Clef();
    bool showClef = false;

    int staff = 1; // the staff that this measure belongs to

    Transpose transpose = Transpose();

    int number = 1;
    int index = 0;

    Duration duration = Duration(); // (hard coded) the duration of the measure
    float beatPosition = 0.0f; // the position in beats in the song

    std::vector<Note*> notes; // the notes contained in this measure
    std::vector<Direction> directions; // the directions contained in this measure
    std::vector<SoundEvent> soundEvents; // the sound events contained in this measure

    int divisions = 1;

//protected: // I don't want to deal with friends and protected and stuff will fix later
    float nextBeatPosition = -1.0f; // -1.0f means it is not a valid value
};