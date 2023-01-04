#pragma once
#include "AndroidDebug.h"
#include "MusicData/Song.h"
#include "ViewModelData.h"
#include "Midi.h"
#include "Callbacks.h"
#include "Settings.h"
#include "RenderableMusicData/RenderableSong.h"
#include "Vec2.h"

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
    void UpdateInstrumentInfo(const InstrumentInfo& info, unsigned int index);
    void SetViewModelData(ViewModelData viewModelData);
    void OnMidiStart();
    void LoadSongFromString(const std::string& string);
    bool OnUpdatePrintLayout();
    int OnCalculateNumPages();
    void RenderMusicToPage(int page);
    void UpdateSettings(const Settings& s) { settings = s; OnLayoutChanged(); }
    void OnLayoutChanged();

    void Render();
    void RenderHorizontalLayout();
    void CalculateRenderForVerticalLayout();
    void CalculateRenderForPagedLayout();
    void RenderWithRenderData();

    void RenderNote(const Note* note, Measure* measure, float measurePositionX, const Staff* staff, float staffYPosition, float instYPosition, int measureNumber, float ls, float mainPositionX, float mainPositionY, int noteIndex);
    void RenderRest(const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY);
    void RenderTabNote(const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY);
    void RenderDirection(const Direction& direction, float measurePositionY, Measure* measure, float measureXPosition, float offsetX, float offsetY);
    void RenderLyric(const Lyric& lyric, float positionY, const Measure* measure, const Note* note, float offsetX, float offsetY);
    void RenderChord(const Chord& chord, float measurePositionY, const Measure* measure, float measureXPosition, float offsetX, float offsetY);
    void RenderTimeSignature(const Measure* measure, float measurePosition, float ls, int lines, float offsetX, float offsetY);
    void RenderKeySignature(const Measure* measure, float measurePosition, float ls, int lines, float offsetX, float offsetY);
    void RenderClef(const Measure* measure, float measurePosition, float ls, int lines, float offsetX, float offsetY);
    void RenderNoteStemAndFlagAndBeam(const Note* note, float notePositionX, float notePositionY);

private:
    void DeleteSong();

private:

    bool layoutCalculated = false;

    bool isUpdating = false;

    Midi midi;

    Paint LinePaint;
    Paint NoteStemPaint;
    Paint NoteBeamPaint;
    Paint BarLinePaint;

    Paint TabSlurPaint;

    Paint TextPaint;
    Paint TabTextPaint;
    Paint InstNameTextPaint;

    int normalColor = 0xff000000;
    int highlightedColor = 0xff1188ee;
    int playedColor = 0xff1188ee;

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

    Song* song = new Song();
    RenderableSong renderableSong = RenderableSong();
    RenderData renderData = RenderData();

    std::vector<Vec2<float>> pagePositions;
    std::vector<Vec2<float>> systemPositions;

    std::string songString = "";

    float displayWidth = 1000.0f;

    float curveStartX = 0.0f;
    float curveStartY = 0.0f;

    float beamStartX = 0.0f;
    float beamStartY = 0.0f;

    bool startRendering = false;

    Settings settings;

    float pageWidth = 1024.0f;
};