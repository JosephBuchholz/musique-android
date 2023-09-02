/**
 * This file contains the definition of the 'MusicPlayer' class.
 */

#ifndef MUSIQUE_MUSICPLAYER_H
#define MUSIQUE_MUSICPLAYER_H

#include "../MusicData/Song.h"

#include "Player.h"
#include "Metronome.h"

/**
 * This class handles all the audio calculations.
 */
class MusicPlayer
{
public:

    MusicPlayer();

    void OnStart();

    void OnPlay();
    void OnStop();

    bool IsPlaying() const { return playing; }

    void OnUpdate(double dt, const std::shared_ptr<Song>& song);

    void Reset();

    void TravelToVisualBeatPosition(float beatPosition, const std::shared_ptr<Song>& song);
    void TravelToSoundBeatPosition(float beatPosition);

    void OnMetronomeToggled(bool state) { metronomeIsOn = state; LOGE_TAG("MusicPlayer", "metronomeIsOn: %d", state); }

    float GetPlayLineBeatPosition() const { return playLineBeatPosition; }
    void SetPlayLineBeatPosition(float beatPosition) { playLineBeatPosition = beatPosition; }

    int GetCurrentMeasure() const { return currentMeasure; }
    void SetCurrentMeasure(int measure) { currentMeasure = measure; }

    void SetVolume(float volume);
    void SetTempoPercentage(float tp);

    void SetOnTempoChangedCallback(void (*callback) (float tempo)) { OnTempoChangedCallback = callback; }

private:
    static void DefaultOnTempoChangedCallback(float tempo) {}

private:

    bool metronomeIsOn = false;

    float playLineBeatPosition = 0.0f; // 1 unit = 1 beat (usually one quarter note)

    float currentMeasureBeatPosition = 0.0f;
    int currentMeasure = 0;

    float tempoPercentage = 1.0f; // this is a speed modifier for the tempo (1.0f just keeps the tempo the same; 2.0f is twice as fast)
    float currentTempo = 120.0f; // beats per minute
    SwingTempo swingTempo;

    std::shared_ptr<Player> player;
    std::shared_ptr<Metronome> metronome;

    bool playing = false;

    void (*OnTempoChangedCallback) (float tempo) = DefaultOnTempoChangedCallback;
};

#endif //MUSIQUE_MUSICPLAYER_H
