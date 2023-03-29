/**
 * This file contains implementations of the non-rendering functions in App.h
 */

#include "App.h"
#include "Utils/Converters.h"
#include "MusicXMLParser/MusicXMLParser.h"
#include "Debuging/Debug.h"

App::App()
{
    LinePaint = Paint();
    LinePaint.strokeWidth = 1.4f;

    NoteStemPaint = Paint();
    NoteStemPaint.strokeWidth = 0.8333f;
    NoteStemPaint.strokeCap = Paint::Cap::Round;

    NoteBeamPaint = Paint();
    NoteBeamPaint.strokeWidth = 5.0f;
    NoteBeamPaint.strokeCap = Paint::Cap::Butt;
    NoteBeamPaint.verticalEnds = true;

    BarLinePaint = Paint();
    BarLinePaint.strokeWidth = 1.25f;
    BarLinePaint.strokeCap = Paint::Cap::Butt;

    HeavyBarLinePaint = Paint();
    HeavyBarLinePaint.strokeWidth = 5.0f;
    HeavyBarLinePaint.strokeCap = Paint::Cap::Butt;

    TabSlurPaint = Paint();
    TabSlurPaint.strokeWidth = 1.25f;

    TiePaint = Paint();
    TiePaint.strokeWidth = 1.25f;

    TextPaint = Paint();
    TextPaint.textSize = 30.0f;

    TabTextPaint = Paint();
    TabTextPaint.textSize = 16.0f;
    TabTextPaint.isTablature = true;

    InstNameTextPaint = Paint();
    InstNameTextPaint.textSize = 9.0f;
    InstNameTextPaint.align = Paint::Align::Right;
}

App::~App()
{
    isUpdating = false;
    LOGI("App Deconstructer");
    ViewModelData viewModelData = ViewModelData();
    viewModelData.playing = playing;
    viewModelData.playLineBeatPosition = playLineBeatPosition;
    viewModelData.currentMeasure = currentMeasure;
    UpdateViewModelData(viewModelData);
    DeleteSong();
    LOGI("App Done Deconstructing");
}

void App::LoadSongFromString(const std::string& string)
{
    isUpdating = false;
    playing = false;
    layoutCalculated = false;
    startRendering = false;
    songUpdated = false;
    playLineBeatPosition = 0.0f;
    currentMeasure = 0;

    LOGI("Loading song from string");
    if (song != nullptr)
    {
        DeleteSong();
    }

    std::string error;
    song = MusicXMLParser::ParseMusicXML(string, error);
    if (!error.empty())
    {
        LOGE("Error when parsing music XML: %s", error.c_str());
    }
    else
    {
        LOGI("There was no error when parsing music XML");
    }

    unsigned int numStaves = 0;
    unsigned int numMeasures = 0;
    unsigned int numNotes = 0;
    if (song != nullptr) {
        for (Instrument *inst : song->instruments) {
            numStaves += inst->staves.size();
            for (Staff *staff : inst->staves) {
                numMeasures += staff->measures.size();
                for (Measure *measure : staff->measures) {
                    numNotes += measure->notes.size();
                }
            }

            //midi.SetVolume(inst->midiInstrument.volume);
        }
    }

    if (error.empty())
        LOGI("instruments: %d, staves: %d, measures: %d, notes: %d", song->instruments.size(), numStaves, numMeasures, numNotes);

    if (song != nullptr) {
        //song->OnUpdate();
        UpdateSongData(song->songData);
    }

    // crash error is caused when isUpdating = true
    if (error.empty())
    {
        isUpdating = true;
        updateRenderData = true;
        //startRendering = true;
    }
}

void App::DeleteSong()
{
    LOGI("Deleting Song Data");
    for (auto* instrument : song->instruments) {
        for (auto* staff : instrument->staves) {
            for (auto* measure : staff->measures) {
                for (Note* note : measure->notes) {
                    delete note;
                    note = nullptr;
                }
                measure->notes.clear();
                measure->directions.clear();
                measure->soundEvents.clear();
                delete measure;
                measure = nullptr;
            }
            staff->measures.clear();
            delete staff;
            staff = nullptr;
        }
        instrument->staves.clear();
        delete instrument;
        instrument = nullptr;
    }
    song->instruments.clear();
    delete song;
    song = nullptr;
}

void App::OnMidiStart()
{
    midi.ChangeInstrument((int)Midi::MidiInstrumentType::AcousticGrandPiano, 0);
}

void App::OnUpdate(double dt)
{
    double dts = dt / 1000.0; // delta time in seconds
    double dtm = dts / 60.0; // delta time in minutes

    /*aTime += dts;
    frames += 1;
    if (aTime >= 1.0f) {
        fps = frames;
        aTime = aTime - 1.0f;
        frames = 0;
    }
    if (icount >= 800000) {
        LOGV("Hello from OnUpdate!: %f, fps: %f", dt, fps);
        jcount += 1;
        icount = 0;
    }
    icount += 1;*/

    if (isUpdating) {

        if (playing) {
            playLineBeatPosition += float(currentTempo * dtm);
        }

        float playLinePosInMeasure = 0.0f;
        if (playing) {
            for (auto *instrument : song->instruments) {
                for (auto *staff : instrument->staves) {
                    int measureIndex = 0;
                    for (auto *measure : staff->measures) {
                        float measureBeatPosition = staff->GetMeasureNextBeatPosition(measureIndex,
                                                                                      playLineBeatPosition);

                        if (playLineBeatPosition > measureBeatPosition &&
                            playLineBeatPosition <=
                            measure->duration.duration + measureBeatPosition) {

                            if (currentMeasure != measureIndex) {
                                /*for (Note *note : staff->measures[currentMeasure]->notes) { // stop all notes from previous measure
                                    if (note->isPlaying) {
                                        note->OnStop();
                                        midi.ChangeInstrument(instrument->midiInstrument.program,
                                                              instrument->midiInstrument.channel);
                                        midi.StopNote(note, measure,
                                                      instrument->midiInstrument.channel);
                                    }
                                }*/

                                currentMeasure = measureIndex;
                                currentMeasureBeatPosition = measureBeatPosition;
                            }

                            for (Note *note : measure->notes) {
                                // POTENTIAL PROBLEM: if the play line skips over the note (like if it lags) then the note won't be played could become a potential problem if the song is really fast
                                if (playLineBeatPosition >=
                                    note->beatPosition + measureBeatPosition &&
                                    playLineBeatPosition <=
                                    note->duration.duration + note->beatPosition +
                                    measureBeatPosition) {
                                    if (!note->isPlaying) {
                                        note->OnPlay();
                                        midi.ChangeInstrument(instrument->midiInstrument.program,
                                                              instrument->midiInstrument.channel);
                                        midi.PlayNote(note, measure,
                                                      instrument->midiInstrument.channel);
                                        updateRenderData = true;
                                    }
                                } else if (note->isPlaying) {
                                    note->OnStop();
                                    midi.ChangeInstrument(instrument->midiInstrument.program,
                                                          instrument->midiInstrument.channel);
                                    midi.StopNote(note, measure,
                                                  instrument->midiInstrument.channel);
                                    updateRenderData = true;
                                }
                            }

                            for (SoundEvent &event : measure->soundEvents) {
                                // NEEDS TO BE FIXED:        \/
                                if (playLineBeatPosition >=
                                    event.beatPosition + measureBeatPosition &&
                                    playLineBeatPosition <=
                                    1.0f/*this*/ + event.beatPosition + measureBeatPosition) {
                                    if (event.tempo != -1.0f) {
                                        currentTempo = event.tempo;
                                        //LOGI("Tempo Changed: %f, events: %i", currentTempo, measure->soundEvents.size());
                                    }
                                }
                            }
                        }

                        measureIndex++;
                    }
                }
            }
        }

        // --- RENDERING ---
        if (startRendering) {
            if (song != nullptr && !songUpdated)
            {
                song->OnUpdate();
                songUpdated = true;
            }
            Render();
        }

    } // if (isUpdating)
}

void App::Render()
{
    switch (settings.musicLayout)
    {
        case Settings::MusicLayout::Horizontal: {
            RenderHorizontalLayout();
            break;
        }
        case Settings::MusicLayout::Vertical: {
            if (!layoutCalculated)
                CalculateRenderForVerticalLayout();

            RenderWithRenderData();

            FrameData frameData = FrameData();
            if (playLineHeight == 0.0f) {
                playLineHeight = 20.0f + 80.0f; //instpos * 80
            }

            frameData.playLinePosition = playLinePosition;
            frameData.playLinePositionY = playLineY;
            frameData.playLineHeight = playLineHeight;
            frameData.playProgress = playLineBeatPosition / song->totalBeatWidth;

            UpdateFrameData(frameData);
            break;
        }
        case Settings::MusicLayout::Paged: {
            if (!layoutCalculated)
                CalculateRenderForPagedLayout();

            RenderWithRenderData();

            //playLinePosition = song->GetPositionXInMeasure(playLineBeatPosition, currentMeasure) + song->GetMeasurePositionX(currentMeasure);
            int currentSystemIndex = song->GetSystemIndex(currentMeasure);
            int currentPageIndex = song->GetPageIndex(currentMeasure);
            playLinePosition = systemPositions[currentSystemIndex].x + song->GetMeasurePositionX(currentMeasure);
            playLineY = systemPositions[currentSystemIndex].y;

            FrameData frameData = FrameData();
            if (playLineHeight == 0.0f) {
                playLineHeight = song->GetSystemHeight(currentMeasure); //instpos * 80
            }

            frameData.playLinePosition = playLinePosition;
            frameData.playLinePositionY = playLineY;
            frameData.playLineHeight = playLineHeight;
            frameData.playProgress = playLineBeatPosition / song->totalBeatWidth;
            frameData.isPlaying = playing;

            UpdateFrameData(frameData);
            break;
        }
        default: {
            //LOGE("This layout is not supported");
            break;
        }
    }
}

void App::RenderHorizontalLayout()
{
    FrameData frameData = FrameData();
    m_RenderData = RenderData();

    // --- MAIN RENDERING ---

    /*
     * An instrument's Y position starts at the first staff line (does not include ledger lines)
     * An instrument's middleHeight is the amount of y space that it takes up below it's y position to the last staff line
     * An instrument's aboveHeight is the amount of y space that it takes up above it's y position
     * An instrument's belowHeight is the amount of y space that it takes up below it's y position + middleHeight
     * An instrument's height is aboveHeight + middleHeight + belowHeight
     */

    float instYPosition = 0.0f;
    if (updateRenderData) {
        Instrument* prevInstrument = nullptr;
        int instrumentIndex = 0;
        for (auto *instrument : song->instruments) {

            if (song->songData.instrumentInfos[instrumentIndex].visible)
            {
                if (prevInstrument != nullptr)
                    instYPosition += prevInstrument->GetMiddleHeight(10.0f, 13.33f, 0, instrument->GetMeasureCount()) +
                                     prevInstrument->GetBelowHeight(10.0f, 13.33f, 0, instrument->GetMeasureCount());
                instYPosition += instrument->GetAboveHeight(10.0f, 13.33f, 0, instrument->GetMeasureCount());

                int staffIndex = 0;
                for (auto *staff : instrument->staves) {
                    float ls = song->displayConstants.lineSpacing;
                    if (staff->type == Staff::StaffType::Tab) {
                        ls = song->displayConstants.tabLineSpacing;
                    }

                    // staff y position
                    float staffYPosition = 0.0f;
                    if (staffIndex == 0) {
                        staffYPosition = 0.0f;
                    } else if (staffIndex == 1) {
                        staffYPosition = instrument->staves[staffIndex-1]->GetMiddleHeight(ls) +
                                         instrument->staves[staffIndex-1]->GetBelowHeight(ls, 0, staff->measures.size()) +
                                         staff->GetAboveHeight(ls, 0, staff->measures.size());
                    }

                    int measureNumber = 0;
                    float measurePosition = 0.0f;
                    int measureRenderCount = 15; // the number of measures that need to be rendered
                    int currentMeasureRenderedCount = 0; // the number of measures that have been rendered
                    for (auto *measure : staff->measures) {

                        if (!(currentMeasureRenderedCount >= measureRenderCount) &&
                            measureNumber >= currentMeasure - (measureRenderCount /
                                                               2)) // render measure in a radius of measureRenderCount/2
                        {
                            float measureWidth = song->GetMeasureWidth(measureNumber);

                            // staff lines
                            for (int i = 0; i < staff->lines; i++) {
                                float endX = measurePosition + measureWidth;
                                m_RenderData.AddLine(
                                        std::make_shared<Line>(measurePosition,
                                             (ls * i) + staffYPosition + instYPosition,
                                             endX,
                                             (ls * i) + staffYPosition + instYPosition,
                                             BarLinePaint));
                            }


                            // measure lines (bar lines)
                            float x = measurePosition;
                            m_RenderData.AddLine(
                                    std::make_shared<Line>(x, 0.0f + staffYPosition + instYPosition, x,
                                         (ls * float(staff->lines - 1)) + staffYPosition +
                                         instYPosition, BarLinePaint));
                            x += song->GetMeasureWidth(measureNumber);
                            m_RenderData.AddLine(
                                    std::make_shared<Line>(x, 0.0f + staffYPosition + instYPosition, x,
                                         (ls * float(staff->lines - 1)) + staffYPosition +
                                         instYPosition, BarLinePaint));

                            //RenderTimeSignature(m_RenderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);
                            //RenderClef(m_RenderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);
                            //RenderKeySignature(m_RenderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);

                            // render directions
                            for (const Direction& direction : measure->directions)
                            {
                                float positionY = staffYPosition + instYPosition - 20.0f;
                                //RenderDirection(m_RenderData, direction, positionY, measure, 0.0f, 0.0f, measurePosition);
                            }

                            int noteIndex = 0;
                            for (Note *note : measure->notes) {
                                RenderNote(m_RenderData, note, measure, measurePosition, staff, staffYPosition + instYPosition, measure->measureWidth, measureNumber, ls, 0.0f, 0.0f, noteIndex);
                                noteIndex++;
                            }

                            // render all chords in this measure
                            for (Chord& chord : measure->chords) {
                                chord.CalculateChordName();
                                float positionY = staffYPosition +
                                                  instYPosition - 40.0f;
                                chord.Render(m_RenderData, measurePosition, positionY);
                            }

                            currentMeasureRenderedCount++;
                        }
                        measurePosition += song->GetMeasureWidth(measureNumber);
                        measureNumber++;
                    } // measures loop
                    staffIndex++;
                } // staves loop

                prevInstrument = instrument;
            }
            instrumentIndex++;
        } // instruments loop

        UpdateRender(m_RenderData);
        updateRenderData = false;
    }

    playLinePosition = song->GetPositionXInSong(playLineBeatPosition, currentMeasure);

    m_RenderData.AddLine(std::make_shared<Line>(0.0f, 20.0f, 0.0f, instYPosition, BarLinePaint));

    if (playLineHeight == 0.0f) {
        playLineHeight = instYPosition + 80.0f;
    }

    frameData.playLinePosition = playLinePosition;
    frameData.playLinePositionY = playLineY;
    frameData.playLineHeight = playLineHeight;
    frameData.playProgress = playLineBeatPosition / song->totalBeatWidth;

    UpdateFrameData(frameData);
}

void App::CalculateRenderForVerticalLayout()
{
    LOGI("Calculating Vertical Layout");
    renderableSong.systems.clear();
    m_RenderData = RenderData();
    //FrameData frameData = FrameData();

    // --- MAIN RENDERING ---

    /*
     * An instrument's Y position starts at the first staff line (does not include ledger lines)
     * An instrument's middleHeight is the amount of y space that it takes up below it's y position to the last staff line
     * An instrument's aboveHeight is the amount of y space that it takes up above it's y position
     * An instrument's belowHeight is the amount of y space that it takes up below it's y position + middleHeight
     * An instrument's height is aboveHeight + middleHeight + belowHeight
     */

    float instYPosition = 0.0f;
    if (updateRenderData) {

        int measureCount = song->GetMeasureCount();

        float totalWidth = 0.0f;
        int startMeasure = 0;
        int endMeasure = 0;
        int systemIndex = 0;
        //RenderableSystem* currentRenderableSystem = nullptr;
        //renderableSong.systems.push_back(RenderableSystem());
        //currentRenderableSystem = &renderableSong.systems[systemIndex];
        for (int i = 0; i < measureCount; i++)
        {
            float width = song->GetMeasureWidth(i);

            if (totalWidth + width > pageWidth)
                endMeasure = i-1;
            else if (i == measureCount-1)
                endMeasure = i;

            if (totalWidth + width > pageWidth || i == measureCount-1) { // start a new system
                renderableSong.systems.emplace_back();
                //currentRenderableSystem = &renderableSong.systems[systemIndex];
                LOGD("endM: %d, startM: %d, i: %d, totalWidth: %f, systemIndex: %d, systemCount: %d", endMeasure, startMeasure, i, totalWidth, systemIndex, renderableSong.systems.size());
                Instrument *prevInstrument = nullptr;
                int instrumentIndex = 0;
                for (auto *instrument: song->instruments) {
                    renderableSong.systems[systemIndex].instruments.emplace_back();

                    if (song->songData.instrumentInfos[instrumentIndex].visible) {
                        if (prevInstrument != nullptr)
                            instYPosition += prevInstrument->GetMiddleHeight(10.0f, 13.33f, 0,
                                                                             instrument->GetMeasureCount()) +
                                             prevInstrument->GetBelowHeight(10.0f, 13.33f, 0,
                                                                            instrument->GetMeasureCount());
                        instYPosition += instrument->GetAboveHeight(10.0f, 13.33f, 0,
                                                                    instrument->GetMeasureCount());

                        int staffIndex = 0;
                        for (Staff* staff : instrument->staves) {
                            renderableSong.systems[systemIndex].instruments[instrumentIndex].staves.emplace_back();
                            float ls = song->displayConstants.lineSpacing;
                            if (staff->type == Staff::StaffType::Tab) {
                                ls = song->displayConstants.tabLineSpacing;
                            }

                            // staff y position
                            float staffYPosition = 0.0f;
                            if (staffIndex == 0) {
                                staffYPosition = 0.0f;
                            } else if (staffIndex == 1) {
                                staffYPosition =
                                        instrument->staves[staffIndex - 1]->GetMiddleHeight(ls) +
                                        instrument->staves[staffIndex - 1]->GetBelowHeight(ls, 0,
                                                                                           staff->measures.size()) +
                                        staff->GetAboveHeight(ls, 0, staff->measures.size());
                            }

                            int measureIndex = startMeasure;
                            int measureNumber = startMeasure;
                            float measurePosition = 0.0f;
                            int measureRenderCount = 15; // the number of measures that need to be rendered
                            int currentMeasureRenderedCount = 0; // the number of measures that have been rendered
                            for (int m = startMeasure; m <= endMeasure; m++) {
                                renderableSong.systems[systemIndex].instruments[instrumentIndex].staves[staffIndex].measures.emplace_back();

                                Measure* measure = staff->measures[m];

                                if (m == currentMeasure)
                                {
                                    playLinePosition = song->GetPositionXInMeasure(playLineBeatPosition, currentMeasure) + measurePosition;
                                    playLineY = instYPosition;
                                }

                                if (!(currentMeasureRenderedCount >= measureRenderCount) &&
                                    measureNumber >= currentMeasure - (measureRenderCount /
                                                                       2)) // render measure in a radius of measureRenderCount/2
                                {
                                    float measureWidth = song->GetMeasureWidth(measureNumber);

                                    // staff lines
                                    for (int i = 0; i < staff->lines; i++) {
                                        float endX = measurePosition + measureWidth;
                                        std::shared_ptr<Line> line = m_RenderData.AddLine(measurePosition,(ls * i) + staffYPosition + instYPosition, endX,(ls * i) + staffYPosition + instYPosition,BarLinePaint);
                                        //renderableSong.systems[systemIndex].instruments[instrumentIndex].staves[staffIndex].measures[measureIndex].staffLines.push_back(std::make_shared<Line>(0.0f, 0.0f, 0.0f, 1.0f));
                                    }

                                    // measure lines (bar lines)
                                    float x = measurePosition;
                                    m_RenderData.AddLine(std::make_shared<Line>(x, 0.0f + staffYPosition + instYPosition, x,
                                                 (ls * float(staff->lines - 1)) + staffYPosition +
                                                 instYPosition, BarLinePaint));
                                    x += song->GetMeasureWidth(measureNumber);
                                    m_RenderData.AddLine(std::make_shared<Line>(x, 0.0f + staffYPosition + instYPosition, x,
                                                 (ls * float(staff->lines - 1)) + staffYPosition +
                                                 instYPosition, BarLinePaint));

                                    /*RenderTimeSignature(m_RenderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);
                                    RenderClef(m_RenderData, measure, measurePosition, ls,
                                               staff->lines, 0.0f, instYPosition + staffYPosition);
                                    RenderKeySignature(m_RenderData, measure, measurePosition, ls,
                                                       staff->lines, 0.0f,
                                                       instYPosition + staffYPosition);*/

                                    // render directions
                                    for (const Direction &direction: measure->directions) {
                                        float positionY = staffYPosition + instYPosition - 20.0f;
                                        //RenderDirection(m_RenderData, direction, positionY, measure,
                                        //                0.0f, 0.0f, measurePosition);
                                    }

                                    int noteIndex = 0;
                                    for (Note *note: measure->notes) {
                                        RenderNote(m_RenderData, note, measure, measurePosition, staff, staffYPosition + instYPosition, measure->measureWidth, measureNumber, ls, 0.0f, 0.0f,
                                                   noteIndex);
                                        noteIndex++;
                                    }

                                    // render all chords in this measure
                                    for (Chord &chord: measure->chords) {
                                        chord.CalculateChordName();
                                        float positionY = staffYPosition +
                                                          instYPosition - 40.0f;
                                        chord.Render(m_RenderData, measurePosition, positionY);
                                    }

                                    currentMeasureRenderedCount++;
                                }
                                measurePosition += song->GetMeasureWidth(measureNumber);
                                measureNumber++;
                                measureIndex++;
                            } // measures loop
                            staffIndex++;
                        } // staves loop

                        prevInstrument = instrument;
                    } // instrument is visible
                    instrumentIndex++;
                } // instruments loop

                totalWidth = width;
                instYPosition += 100.0f;
                startMeasure = i;
                systemIndex++;
            }
            else
                totalWidth += width;
        }

        //UpdateRender(m_RenderData);
        //updateRenderData = false;
    }

    m_RenderData.AddLine(std::make_shared<Line>(0.0f, 20.0f, 0.0f, instYPosition, BarLinePaint));

    layoutCalculated = true;
}

void App::CalculateRenderForPagedLayout()
{
    LOGI("Calculating Paged Layout");
    renderableSong.systems.clear();
    m_RenderData = RenderData();

    pagePositions.clear();
    systemPositions.clear();

    int totalPages = OnCalculateNumPages();
    int pageIndex = 0;

    float pageX = 0.0f;
    float pageY = 0.0f;

    while (pageIndex < totalPages)
    {
        if (pageIndex != 0)
        {
            if (settings.pagesOrientation == Orientation::Vertical)
                pageY += song->displayConstants.pageHeight + 80.0f;
            else if (settings.pagesOrientation == Orientation::Horizontal)
                pageX += song->displayConstants.pageWidth + 80.0f;
        }

        // ---- RENDER ----
        RenderMusicToPage(pageIndex, m_RenderData, pageX, pageY);

        pagePositions.emplace_back(pageX, pageY);
        pageIndex++;

        // debug page lines
        m_RenderData.AddLine(std::make_shared<Line>(pageX, pageY, pageX + song->displayConstants.pageWidth, pageY, BarLinePaint));
        m_RenderData.AddLine(std::make_shared<Line>(pageX, pageY, pageX, pageY + song->displayConstants.pageHeight, BarLinePaint));

        m_RenderData.AddLine(std::make_shared<Line>(pageX, pageY + song->displayConstants.pageHeight, pageX + song->displayConstants.pageWidth, pageY + song->displayConstants.pageHeight, BarLinePaint));
        m_RenderData.AddLine(std::make_shared<Line>(pageX + song->displayConstants.pageWidth, pageY, pageX + song->displayConstants.pageWidth, pageY + song->displayConstants.pageHeight, BarLinePaint));
    }

#if SHOW_BOUNDING_BOXES

    song->UpdateBoundingBoxes(pagePositions, systemPositions);
    song->RenderBoundingBoxes(m_RenderData, pagePositions, systemPositions);
#endif

    layoutCalculated = true;
}

void App::RenderMusicToPage(int page, RenderData& pageRenderData, float pageX, float pageY) {

    // --- MAIN RENDERING ---

    LOGI("Rendering pages for printing");

    // --- MAIN RENDERING ---

    /*
     * An instrument's Y position starts at the first staff line (does not include ledger lines)
     * An instrument's middleHeight is the amount of y space that it takes up below it's y position to the last staff line
     * An instrument's aboveHeight is the amount of y space that it takes up above it's y position
     * An instrument's belowHeight is the amount of y space that it takes up below it's y position + middleHeight
     * An instrument's height is aboveHeight + middleHeight + belowHeight
     */

    if (updateRenderData) {

        int measureCount = song->GetMeasureCount();

        int startingMeasureIndex = song->GetFirstMeasureOnPage(page);

        int startMeasure = startingMeasureIndex;
        int endMeasure = startingMeasureIndex-1;

        int systemIndex = song->GetSystemIndex(startingMeasureIndex);

        LOGD("page: %d, start measure: %d, systemIndex: %d", page, startingMeasureIndex, systemIndex);

        bool drewInstNames = false;

        float systemPositionX = pageX + song->displayConstants.leftMargin + song->systems[systemIndex].layout.systemLeftMargin;
        float systemPositionY = pageY + song->displayConstants.topMargin + song->systems[systemIndex].layout.topSystemDistance;

        if (page == 0)
        {
            RenderCredits(pageRenderData, song->displayConstants, song->credits, pageX, pageY);
        }

        for (int i = startingMeasureIndex; i < measureCount; i++)
        {
            bool startNewSystem = song->DoesMeasureStartNewSystem(i);
            bool startNewPage = song->DoesMeasureStartNewPage(i);

            if (i == startingMeasureIndex) // first time
                startNewPage = false, startNewSystem = false;
            //    startNewPage = true, startNewSystem = true;

            if ((startNewSystem || i == measureCount-1) && i != 0) { // render system
                startMeasure = endMeasure+1;
                endMeasure = i-1;
                renderableSong.systems.emplace_back();
                //currentRenderableSystem = &renderableSong.systems[systemIndex];
                //LOGD("endM: %d, startM: %d, i: %d, totalWidth: %f, systemIndex: %d, systemCount: %d", endMeasure, startMeasure, i, totalWidth, systemIndex, renderableSong.systems.size());
                Instrument *prevInstrument = nullptr;
                int instrumentIndex = 0;
                float instYPosition = systemPositionY;
                for (auto *instrument: song->instruments) {
                    renderableSong.systems[systemIndex].instruments.emplace_back();

                    if (song->songData.instrumentInfos[instrumentIndex].visible) {

                        if (instrumentIndex != 0 && !instrument->staves.empty())
                        {
                            if (prevInstrument != nullptr)
                                instYPosition += prevInstrument->GetMiddleHeight(10.0f, 13.33f, 0,instrument->GetMeasureCount());

                            instYPosition += instrument->staves.front()->measures[startMeasure]->staffDistance;
                        }

                        if (song->instruments.size() > 1)
                        {
                            float textPositionY = instYPosition + (instrument->GetMiddleHeight(song->displayConstants.lineSpacing, song->displayConstants.tabLineSpacing, 0, 1) / 2.0f);
                            if (!drewInstNames)
                            {
                                pageRenderData.AddText(Text(instrument->name.string, systemPositionX - 10.0f, textPositionY, InstNameTextPaint));
                            }
                            else
                            {
                                pageRenderData.AddText(Text(instrument->nameAbbreviation.string, systemPositionX - 10.0f, textPositionY, InstNameTextPaint));
                            }
                        }

                        int staffIndex = 0;
                        for (Staff* staff : instrument->staves) {
                            renderableSong.systems[systemIndex].instruments[instrumentIndex].staves.emplace_back();
                            float ls = song->displayConstants.lineSpacing;
                            if (staff->type == Staff::StaffType::Tab) {
                                ls = song->displayConstants.tabLineSpacing;
                            }

                            // staff y position
                            float staffYPosition = 0.0f;
                            if (staffIndex == 0) {
                                staffYPosition = 0.0f;
                            } else if (staffIndex >= 1) {
                                staffYPosition += staff->measures[startMeasure]->staffDistance +
                                                  instrument->staves[staffIndex - 1]->GetMiddleHeight(ls);
                            }

                            RenderLineOfMeasures(pageRenderData, startMeasure, endMeasure, song->systems[systemIndex], staff, systemPositionX, staffYPosition + instYPosition, ls);

                            staffIndex++;
                        } // staves loop

                        prevInstrument = instrument;
                    } // instrument is visible

                    instrumentIndex++;
                } // instruments loop

                pageRenderData.AddLine(std::make_shared<Line>(systemPositionX, systemPositionY, systemPositionX, instYPosition + 20.0f, BarLinePaint));

                drewInstNames = true;

                startMeasure = i;
                systemIndex++;

                if (prevInstrument != nullptr)
                    systemPositionY = instYPosition + song->systems[systemIndex].layout.systemDistance + prevInstrument->GetMiddleHeight(10.0f, 13.33f, startMeasure, endMeasure);
                systemPositionX = pageX + song->displayConstants.leftMargin + song->systems[systemIndex].layout.systemLeftMargin;
            } // system loop (sort of)

            if (systemIndex >= systemPositions.size())
                systemPositions.emplace_back(systemPositionX, systemPositionY);

            if (song->GetPageIndex(i) > page) // don't render the next page
            {
                break;
            }
        }
    }
}

void App::RenderWithRenderData()
{
    //LOGE("staffLines count: %d", renderableSong.systems[0].instruments[0].staves[0].measures[0].staffLines.size());
    /*int i = 0;
    for (const std::shared_ptr<Line>& line : renderableSong.systems[0].instruments[1].staves[0].measures[0].staffLines)
    {
        if (line == nullptr)
        {
            LOGV("is nullptr: %d", i);
            continue;
        }

        line->paint.color += 20;
        i++;
    }*/
    if (updateRenderData)
    {
        UpdateRender(m_RenderData);
        updateRenderData = false;
    }
}


void App::SetViewModelData(ViewModelData viewModelData)
{
    playing = viewModelData.playing;
    playLineBeatPosition = viewModelData.playLineBeatPosition;
    currentMeasure = viewModelData.currentMeasure;
}

void App::OnPlayProgressChanged(float progress)
{
    playLineBeatPosition = song->totalBeatWidth * progress;
}

void AddLine(RenderData& renderData, float startX, float startY, float endX, float endY, float offsetX, float offsetY, Paint paint)
{
    renderData.AddLine(
            std::make_shared<Line>(startX + offsetX,
                                   startY + offsetY,
                                   endX + offsetX,
                                   endY + offsetY,
                                   paint));
}

bool App::OnUpdatePrintLayout()
{
    LOGI("ON UPDATE PRINT LAYOUT!!");
    PrintRenderData printRenderData = PrintRenderData();
    bool layoutChanged = true;

    int totalPages = OnCalculateNumPages();
    int pageIndex = 0;
    while (pageIndex < totalPages)
    {
        RenderData pageRenderData = RenderData();
        updateRenderData = true;
        RenderMusicToPage(pageIndex, pageRenderData, 0.0f, 0.0f);
        printRenderData.pages.push_back(pageRenderData);
        pageIndex++;
    }

    UpdatePrintRenderData(printRenderData);
    return layoutChanged;
}

int App::OnCalculateNumPages()
{
    int numPages = song->GetNumPages();

    if (numPages == 0)
        LOGE("NUM PAGES IS 0");

    LOGI("There are %d pages", numPages);

    return numPages;
}

void App::UpdateInstrumentInfo(const InstrumentInfo& info, unsigned int index)
{
    if (song && song->songData.instrumentInfos.size() > index) {
        if (song->songData.instrumentInfos[index].visible != info.visible) // visible field changed, so update render
            updateRenderData = true;
        song->songData.instrumentInfos[index] = info;
    }
}

void App::OnLayoutChanged()
{
    song->OnLayoutChanged(settings.musicLayout);
    renderableSong = RenderableSong();
    m_RenderData = RenderData(); // reduntant
    updateRenderData = true;
    layoutCalculated = false;

    LOGI("NEW layout: %d", (int)settings.musicLayout);
}
