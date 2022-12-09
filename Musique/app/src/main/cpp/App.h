#pragma once
#include "AndroidDebug.h"
#include "MusicData/Song.h"
#include "ViewModelData.h"
#include "Midi.h"
#include "Callbacks.h"
#include "Settings.h"

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
    void RenderMusicToPage(RenderData& renderData, int page);

    void Render();
    void RenderHorizontalLayout();
    void RenderVerticalLayout();

    void RenderNote(RenderData& renderData, const Note* note, Measure* measure, float measurePositionX, const Staff* staff, float staffYPosition, float instYPosition, int measureNumber, float ls, float mainPositionX, float mainPositionY, int noteIndex);
    void RenderRest(RenderData& renderData, const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY);
    void RenderTabNote(RenderData& renderData, const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY);
    void RenderDirection(RenderData& renderData, const Direction& direction, float positionY, Measure* measure, float measureXPosition, float offsetX, float offsetY);
    void RenderLyric(RenderData& renderData, const Lyric& lyric, float positionY, const Measure* measure, const Note* note, float offsetX, float offsetY);
    void RenderChord(RenderData& renderData, const Chord& chord, float positionY, const Measure* measure, float measureXPosition, float offsetX, float offsetY);
    void RenderTimeSignature(RenderData& renderData, const Measure* measure, float measurePosition, float ls, float offsetX, float offsetY);
    void RenderKeySignature(RenderData& renderData, const Measure* measure, float measurePosition, float ls, int lines, float offsetX, float offsetY);
    void RenderClef(RenderData& renderData, const Measure* measure, float measurePosition, float ls, int lines, float offsetX, float offsetY);
    void RenderNoteStemAndFlagAndBeam(RenderData& renderData, const Note* note, float notePositionX, float notePositionY);

private:
    void DeleteSong();

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

    Song* song = new Song();
    std::string songString = "";

    float displayWidth = 1000.0f;

    float curveStartX = 0.0f;
    float curveStartY = 0.0f;

    float beamStartX = 0.0f;
    float beamStartY = 0.0f;

    float lineSpacing = 10.0f;
    float tabLineSpacing = 13.33f;

    bool startRendering = false;

    Settings settings;

    float pageWidth = 1024.0f;
};