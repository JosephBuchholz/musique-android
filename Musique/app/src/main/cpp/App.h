/**
 * This file defines the class `App` which acts like an interface between Kotlin and C++.
 */

#ifndef MUSIQUE_APP_H
#define MUSIQUE_APP_H

#include "MusicData/Song.h"
#include "ViewModelData.h"
#include "Callbacks.h"
#include "Settings.h"
#include "Collisions/Vec2.h"
#include "Events/InputEvent.h"

#include "MusicRenderer.h"
#include "MusicPlayer.h"

/**
 * This class acts like an interface between Kotlin and C++.
 */
class App {

public:

    App();
    ~App();

    void StartRendering() { startRendering = true; }
    void StopRendering() { startRendering = false; }
    void OnUpdate(double dt);
    void OnPlayButtonToggled(bool state) { if (state) { musicPlayer->OnPlay(); } else { musicPlayer->OnStop(); } }
    void OnResetButtonPressed() { musicPlayer->Reset(); }
    void OnPlayProgressChanged(float progress);
    void UpdateInstrumentInfo(const InstrumentInfo& info, unsigned int index);
    void SetViewModelData(ViewModelData viewModelData);
    void OnMidiStart();
    void LoadSongFromString(const std::string& string);
    bool OnUpdatePrintLayout();
    void UpdateSettings(const Settings& s) { settings = s; OnLayoutChanged(); }
    void OnLayoutChanged();
    void OnVolumeChanged(float volume) { if (musicPlayer) musicPlayer->SetVolume(volume); }

    void OnMetronomeToggled(bool state) { if (musicPlayer) musicPlayer->OnMetronomeToggled(state); }

    void OnInputEvent(const InputEvent& event);

    int OnCalculateNumPages();

private:
    void DeleteSong();

private:

    std::shared_ptr<MusicRenderer> musicRenderer;
    std::shared_ptr<MusicPlayer> musicPlayer;

    bool isUpdating = false;

    int jcount = 0;
    int icount = 0;
    float aTime = 0.0f;
    float fps = 0.0f;
    int frames = 0;

    float playLinePosition = 0.0f;
    float playLineHeight = 0.0f;
    float playLineY = 0.0f;


    std::shared_ptr<Song> song;
    bool songUpdated = false;



    std::string songString = "";

    float displayWidth = 1000.0f;

    float curveStartX = 0.0f;
    float curveStartY = 0.0f;

    float beamStartX = 0.0f;
    float beamStartY = 0.0f;

    bool startRendering = false;

    Settings settings;

    float pageWidth = 1024.0f;

    Vec2<float> pointerPosition = { 0.0f, 0.0f };
};

#endif // MUSIQUE_APP_H