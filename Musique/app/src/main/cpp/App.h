#pragma once
#include "AndroidDebug.h"
#include "MusicData/SongData.h"
#include "ViewModelData.h"
#include "Midi.h"
#include "Callbacks.h"

class App {

public:

    App();
    ~App();

    void StartRendering() { startRendering = true; }
    void StopRendering() { startRendering = false; }
    void OnUpdate(double dt);
    void OnPlayButtonToggled(bool state) { playing = state; }
    void OnResetButtonPressed() { playLineBeatPosition = 0.0f; }
    void OnPlayProgressChanged(float progress);
    void SetViewModelData(ViewModelData viewModelData);
    void OnMidiStart();
    void LoadSongFromString(const std::string& string);

    void RenderDirection(RenderData& renderData, const Direction& direction, float positionY, Measure* measure);
    void RenderLyric(RenderData& renderData, const Lyric& lyric, float positionY, Measure* measure, Note* note);

private:
    void DeleteSongData();

private:

    bool isUpdating = false;

    Midi midi;

    Paint LinePaint;
    Paint NoteStemPaint;
    Paint NoteBeamPaint;
    Paint BarLinePaint;

    Paint TabSlurPaint;

    Paint TextPaint;
    Paint TabTextPaint;

    int normalColor = 0xff000000;
    int highlightedColor = 0xff1188ee;
    int playedColor = 0xff1188ee;

    enum class DisplayMode {
        None = 0, Linear, Vertical
    };

    DisplayMode displayMode = DisplayMode::Linear;

    int jcount = 0;
    int icount = 0;
    float aTime = 0.0f;
    float fps = 0.0f;
    int frames = 0;

    float playLinePosition = 0.0f;
    float playLineBeatPosition = 0.0f; // 1 unit = 1 beat (usually one quarter note)
    float playLineHeight = 0.0f;
    float playLineY = 0.0f;

    bool updateRenderData = true; // weather the sheet music needs to be updated(rendered) again


    float currentMeasureBeatPosition = 0.0f;
    int currentMeasure = 0;

    bool playing = false;

    float currentTempo = 120.0f; // beats per minute

    SongData* songData = new SongData();
    std::string songString = "";

    float displayWidth = 1000.0f;

    bool startRendering = false;
};