#pragma once

class SoundEvent {
public:

    float beatPosition = 0.0f; // the position of the note in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the note in the song(not counting repeats) in beats(quarter notes)

    float tempo = 120.0f; // beat per minute

    int pan = 0;

    float dynamics = 1.0f; // 1.0 is about a forte
    int velocity = dynamics * 90; // 90 is about a forte
};