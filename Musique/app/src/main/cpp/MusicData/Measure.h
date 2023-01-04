#pragma once
#include "Note.h"
#include "TimeSignature.h"
#include "KeySignature.h"
#include "Clef.h"
#include "Transpose.h"
#include "Directions/Direction.h"
#include "SoundEvent.h"
#include "Chords/Chord.h"
#include <vector>

class Measure {
    friend class Staff;
    friend class Song;
    friend class MusicXMLParser;

public:

    float GetMiddleHeight(float staffLineCount, float lineSpacing);
    float GetAboveHeight(float staffLineCount, float lineSpacing);
    float GetBelowHeight(float staffLineCount, float lineSpacing);
    float GetTotalHeight(float staffLineCount, float lineSpacing);
    float GetBeginningWidth() const;

    float CalculateMinWidth(float notesWidth) const;

    float GetNotePositionInMeasure(float width, int noteIndex) const;
    float GetKeySignaturePositionInMeasure(float width) const;
    float GetTimeSignaturePositionInMeasure(float width) const;
    float GetClefPositionInMeasure(float width) const;

    float GetPitchYPosition(Pitch pitch) const;
    float CalculateNoteYPositionRelativeToMeasure(int noteIndex) const;
    int GetLetterNumber(const std::string& s) const;

    float GetClefLineYPosition(MusicDisplayConstants displayConstants, int lines) const;

    float GetPlayLinePositionInMeasure(float playLineBeatPositionInMeasure, float width) {
        return (playLineBeatPositionInMeasure / duration.duration) * width; // <-- need to be better
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

protected:
    void CalculateAsPaged(const MusicDisplayConstants& displayConstants);

public:

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

    float staffDistance = 0.0f;

    Duration duration = Duration(); // (hard coded) the duration of the measure
    float beatPosition = 0.0f; // the position in beats in the song

    std::vector<Note*> notes; // the notes contained in this measure
    std::vector<Direction> directions; // the directions contained in this measure
    std::vector<SoundEvent> soundEvents; // the sound events contained in this measure
    std::vector<Chord> chords; // the chords contained in this measure

    int divisions = 1;

    float measureWidth = 0.0f;

protected:
    // -- Constants In MusicXML --

    // as stated in the file (these are in tenths)
    float defaultMeasureWidth = -1.0f; // negative means that it wasn't mentioned in the file

    float defStaffDistance = -1.0f; // if negative then use the default value in displayConstants

    bool startNewSystem = false;
    bool startNewPage = false;

    // -- Other --

    float nextBeatPosition = -1.0f; // -1.0f means it is not a valid value
};