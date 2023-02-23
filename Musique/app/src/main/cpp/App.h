/**
 * This file defines the class `App` which controls how the sheet music is displayed and acts like an interface between Kotlin and C++
 */

#pragma once
#include "AndroidDebug.h"
#include "MusicData/Song.h"
#include "ViewModelData.h"
#include "Midi.h"
#include "Callbacks.h"
#include "Settings.h"
#include "RenderableMusicData/RenderableSong.h"
#include "Vec2.h"

/**
 * This class acts like an interface between Kotlin and C++.
 * This class handles most of the rendering (and audio) calculations
 * to then be packaged up (as RenderData) and sent to Kotlin for the actual rendering.
 */
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
    void RenderMusicToPage(int page, RenderData& pageRenderData, float pageX, float pageY);
    void UpdateSettings(const Settings& s) { settings = s; OnLayoutChanged(); }
    void OnLayoutChanged();

    void Render();
    void RenderHorizontalLayout();
    void CalculateRenderForVerticalLayout();
    void CalculateRenderForPagedLayout();
    void RenderWithRenderData();

private:
    void RenderLineOfMeasures(RenderData& renderData, unsigned int startMeasure, unsigned int endMeasure, const System& system, Staff* staff, float systemPositionX, float staffPositionY, float lineSpacing);

    void RenderNote(RenderData& renderData, const Note* note, Measure* measure, float measurePositionX, const Staff* staff, float measurePositionY, int measureNumber, float ls, float mainPositionX, float mainPositionY, int noteIndex);
    void RenderRest(RenderData& renderData, const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY);
    void RenderTabNote(RenderData& renderData, const Note* note, const Staff* staff, float measurePositionX, int lines, float ls, float offsetX, float offsetY);
    void RenderDirection(RenderData& renderData, const Direction& direction, float measurePositionY, Measure* measure, float measureXPosition, float offsetX, float offsetY);
    void RenderLyric(RenderData& renderData, const Lyric& lyric, float notePositionX, float measurePositionY, float offsetX, float offsetY);
    void RenderChord(RenderData& renderData, const Chord& chord, float measurePositionY, const Measure* measure, float measureXPosition, float offsetX, float offsetY);
    void RenderTimeSignature(RenderData& renderData, const Measure* measure, float positionX, float ls, int lines, float offsetX, float offsetY);
    void RenderKeySignature(RenderData& renderData, const Measure* measure, float positionX, float ls, int lines, float offsetX, float offsetY);
    void RenderClef(RenderData& renderData, const Measure* measure, float positionX, float ls, int lines, float offsetX, float offsetY);

    void RenderNoteFlag(RenderData& renderData, const Note* note, float notePositionX, float notePositionY);
    void RenderNoteStem(RenderData& renderData, const Note* note, float notePositionX, float notePositionY);

    void RenderAugmentationDots(RenderData& renderData, const std::vector<AugmentationDot>& dots, float notePositionX, float notePositionY);

    void RenderBarlines(RenderData& renderData, const std::vector<Barline>& barlines, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing);
    void RenderBarline(RenderData& renderData, const Barline& barline, float barlinePositionX, float barlinePositionY, float barlineHeight, int lineCount, float lineSpacing);

    void RenderMultiMeasureRest(RenderData& renderData, unsigned int measureRestCount, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing);

    void RenderCredits(RenderData& renderData, const MusicDisplayConstants& displayConstants, const std::vector<Credit>& credits, float pageX, float pageY);

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
    Paint HeavyBarLinePaint;

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
    bool songUpdated = false;

    RenderableSong renderableSong = RenderableSong();
    RenderData m_RenderData = RenderData();

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