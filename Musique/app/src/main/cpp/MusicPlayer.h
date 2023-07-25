/**
 * This file contains the definition of the 'MusicPlayer' class.
 */

#ifndef MUSIQUE_MUSICPLAYER_H
#define MUSIQUE_MUSICPLAYER_H

#include "MusicData/Song.h"

#include "Player.h"
#include "Metronome.h"

/**
 * This class handles all the audio calculations.
 */
class MusicPlayer
{
public:

    MusicPlayer();

    void OnUpdate(double dt, std::shared_ptr<Song> song);

    void Reset();

public:

    bool playing = false;

    bool metronomeIsOn = false;

    float playLineBeatPosition = 0.0f; // 1 unit = 1 beat (usually one quarter note)

    float currentMeasureBeatPosition = 0.0f;
    int currentMeasure = 0;

    float currentTempo = 120.0f; // beats per minute
    SwingTempo swingTempo;

    std::shared_ptr<Player> player;
    std::shared_ptr<Metronome> metronome;
};

#endif //MUSIQUE_MUSICPLAYER_H
