/**
 * This file contains implementations of the functions in App.h
 */

#include "App.h"
#include "Utils/Converters.h"
#include "MusicXMLParser/MusicXMLParser.h"


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

    BarLinePaint = Paint();
    BarLinePaint.strokeWidth = 1.25f;
    BarLinePaint.strokeCap = Paint::Cap::Butt;

    HeavyBarLinePaint = Paint();
    HeavyBarLinePaint.strokeWidth = 5.0f;
    HeavyBarLinePaint.strokeCap = Paint::Cap::Butt;

    TabSlurPaint = Paint();
    TabSlurPaint.strokeWidth = 1.25f;

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
        song->OnUpdate();
        UpdateSongData(song->songData);
    }

    // crash error is caused when isUpdating = true
    if (error.empty())
    {
        isUpdating = true;
        updateRenderData = true;
        startRendering = true;
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

                            RenderTimeSignature(m_RenderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);
                            RenderClef(m_RenderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);
                            RenderKeySignature(m_RenderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);

                            // render directions
                            for (const Direction& direction : measure->directions)
                            {
                                float positionY = staffYPosition + instYPosition - 20.0f;
                                RenderDirection(m_RenderData, direction, positionY, measure, 0.0f, 0.0f, measurePosition);
                            }

                            int noteIndex = 0;
                            for (Note *note : measure->notes) {
                                RenderNote(m_RenderData, note, measure, measurePosition, staff, staffYPosition + instYPosition, measureNumber, ls, 0.0f, 0.0f, noteIndex);
                                noteIndex++;
                            }

                            // render all chords in this measure
                            for (Chord& chord : measure->chords) {
                                chord.CalculateChordName();
                                float positionY = staffYPosition +
                                                  instYPosition - 40.0f;
                                RenderChord(m_RenderData, chord, positionY, measure, 0.0f, 0.0f, measurePosition);
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

                                    RenderTimeSignature(m_RenderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);
                                    RenderClef(m_RenderData, measure, measurePosition, ls,
                                               staff->lines, 0.0f, instYPosition + staffYPosition);
                                    RenderKeySignature(m_RenderData, measure, measurePosition, ls,
                                                       staff->lines, 0.0f,
                                                       instYPosition + staffYPosition);

                                    // render directions
                                    for (const Direction &direction: measure->directions) {
                                        float positionY = staffYPosition + instYPosition - 20.0f;
                                        RenderDirection(m_RenderData, direction, positionY, measure,
                                                        0.0f, 0.0f, measurePosition);
                                    }

                                    int noteIndex = 0;
                                    for (Note *note: measure->notes) {
                                        RenderNote(m_RenderData, note, measure, measurePosition, staff, staffYPosition + instYPosition, measureNumber, ls, 0.0f, 0.0f,
                                                   noteIndex);
                                        noteIndex++;
                                    }

                                    // render all chords in this measure
                                    for (Chord &chord: measure->chords) {
                                        chord.CalculateChordName();
                                        float positionY = staffYPosition +
                                                          instYPosition - 40.0f;
                                        RenderChord(m_RenderData, chord, positionY, measure, 0.0f,
                                                    0.0f, measurePosition);
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

    if (updateRenderData) { // was true

        int measureCount = song->GetMeasureCount();

        int startingMeasureIndex = song->GetFirstMeasureOnPage(page);

        int startMeasure = startingMeasureIndex;
        int endMeasure = startingMeasureIndex-1;

        int systemIndex = song->GetSystemIndex(startingMeasureIndex);

        LOGD("page: %d, start measure: %d, systemIndex: %d", page, startingMeasureIndex, systemIndex);

        bool drewInstNames = false;

        float systemPositionX = pageX + song->displayConstants.leftMargin + song->systemLayouts[systemIndex].systemLeftMargin;
        float systemPositionY = pageY + song->displayConstants.topMargin + song->systemLayouts[systemIndex].topSystemDistance;

        if (page == 0)
        {
            for (const Credit &credit: song->credits)
            {
                if (credit.pageNumber == 1)
                {
                    float positionX = credit.words.positionX + pageX;
                    float positionY = (song->displayConstants.pageHeight - credit.words.positionY) + pageY; // get coordinate from top instead of bottom of page

                    Paint paint = Paint();
                    if (credit.words.fontStyle == FontStyle::Italic)
                        paint.isItalic = true;
                    if (credit.words.fontWeight == FontWeight::Bold)
                        paint.isBold = true;

                    paint.textSize = credit.words.fontSize.size;

                    if (credit.words.justify == Justify::Center)
                        paint.align = Paint::Align::Center;
                    else if (credit.words.justify == Justify::Left)
                        paint.align = Paint::Align::Left;
                    else if (credit.words.justify == Justify::Right)
                        paint.align = Paint::Align::Right;

                    pageRenderData.AddText(Text(credit.words.text, positionX, positionY, paint));

                }
            }
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
                            if (!drewInstNames)
                            {
                                pageRenderData.AddText(Text(instrument->name.string, systemPositionX - 10.0f, instYPosition, InstNameTextPaint));
                            }
                            else
                            {
                                pageRenderData.AddText(Text(instrument->nameAbbreviation.string, systemPositionX - 10.0f, instYPosition, InstNameTextPaint));
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

                            RenderLineOfMeasures(pageRenderData, startMeasure, endMeasure, staff, systemPositionX, staffYPosition + instYPosition, ls);

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
                    systemPositionY = instYPosition + song->systemLayouts[systemIndex].systemDistance + prevInstrument->GetMiddleHeight(10.0f, 13.33f, startMeasure, endMeasure);
                systemPositionX = pageX + song->displayConstants.leftMargin + song->systemLayouts[systemIndex].systemLeftMargin;
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

void App::RenderLineOfMeasures(RenderData& renderData, unsigned int startMeasure, unsigned int endMeasure, Staff* staff, float systemPositionX, float staffPositionY, float lineSpacing)
{
    bool multiMeasureRest = false; // whether the measure is part of a multi measure rest
    unsigned int numberOfMeasuresInMultiMeasureRest = 0; // number of measures left in multi measure rest
    unsigned int measureThatStartedMultiMeasureRest = 0; // the index of the measure that started the multi measure rest

    int measureIndex = startMeasure;
    int measureNumber = startMeasure;
    float measurePosition = systemPositionX;
    int measureRenderCount = 15; // the number of measures that need to be rendered
    int currentMeasureRenderedCount = 0; // the number of measures that have been rendered
    for (int m = startMeasure; m <= endMeasure; m++) {
        //renderableSong.systems[systemIndex].instruments[instrumentIndex].staves[staffIndex].measures.emplace_back();

        Measure* measure = staff->measures[m];
        //LOGE("i: %d, width: %f, pos: %f", m, measure->measureWidth, measurePosition);

        if (m == currentMeasure)
        {
            playLinePosition = song->GetPositionXInMeasure(playLineBeatPosition, currentMeasure) + measurePosition;
        }

        if (multiMeasureRest)
        {
            if (measureIndex - measureThatStartedMultiMeasureRest < numberOfMeasuresInMultiMeasureRest) // this measure is part of the multi measure rest
            {
                // do nothing
            }
            else // this measure is not part of the multi measure rest (so it has ended)
            {
                multiMeasureRest = false;
                numberOfMeasuresInMultiMeasureRest = 0;
                measureThatStartedMultiMeasureRest = 0;
            }
        }

        if (!(currentMeasureRenderedCount >= measureRenderCount) &&
            measureNumber >= currentMeasure - (measureRenderCount / 2) &&
            !multiMeasureRest) // render measure in a radius of measureRenderCount/2
        {
            if (measure->startsMultiMeasureRest)
            {
                measure->measureWidth = 80.0f;
                multiMeasureRest = true;
                numberOfMeasuresInMultiMeasureRest = measure->numberOfMeasuresInMultiMeasureRest;
                measureThatStartedMultiMeasureRest = measureIndex;

                RenderMultiMeasureRest(renderData, measure->numberOfMeasuresInMultiMeasureRest, measurePosition, staffPositionY, measure->measureWidth, staff->lines, lineSpacing);
            }

            //float measureWidth = song->GetMeasureWidth(measureNumber);

            // staff lines
            for (int j = 0; j < staff->lines; j++) {
                float endX = measurePosition + measure->measureWidth;
                std::shared_ptr<Line> line = renderData.AddLine(measurePosition, (lineSpacing * (float)j) + staffPositionY, endX,(lineSpacing * (float)j) + staffPositionY,BarLinePaint);
                //renderableSong.systems[systemIndex].instruments[instrumentIndex].staves[staffIndex].measures[measureIndex].staffLines.push_back(std::make_shared<Line>(0.0f, 0.0f, 0.0f, 1.0f));
            }

            //LOGV("barline count: %d, measure index: %d", measure->barlines.size(), measure->index);
            RenderBarlines(renderData, measure->barlines, measurePosition, staffPositionY, measure->measureWidth, staff->lines, lineSpacing);
            // measure lines (bar lines)
            float x = measurePosition;
            renderData.AddLine(std::make_shared<Line>(x, 0.0f + staffPositionY, x,
                                                        (lineSpacing * float(staff->lines - 1)) + staffPositionY, BarLinePaint));
            x += measure->measureWidth;
            renderData.AddLine(std::make_shared<Line>(x, 0.0f + staffPositionY, x,
                                                        (lineSpacing * float(staff->lines - 1)) + staffPositionY, BarLinePaint));


            RenderTimeSignature(renderData, measure, measurePosition, lineSpacing, staff->lines, 0.0f, staffPositionY);

            RenderClef(renderData, measure, measurePosition, lineSpacing, staff->lines, 0.0f, staffPositionY);

            RenderKeySignature(renderData, measure, measurePosition, lineSpacing,staff->lines, 0.0f,staffPositionY);

            // render directions
            for (const Direction &direction: measure->directions) {
                float measurePositionY = staffPositionY;
                RenderDirection(renderData, direction, measurePositionY, measure,
                                measurePosition, 0.0f, 0.0f);
            }

            if (!measure->startsMultiMeasureRest)
            {
                int noteIndex = 0;
                for (Note *note: measure->notes) {
                    RenderNote(renderData, note, measure, measurePosition, staff, staffPositionY, measureNumber, lineSpacing, 0.0f, 0.0f,
                               noteIndex);
                    noteIndex++;
                }

                // render all chords in this measure
                for (Chord &chord: measure->chords) {
                    chord.CalculateChordName();
                    float measurePositionY = staffPositionY;
                    RenderChord(renderData, chord, measurePositionY, measure, measurePosition,
                                0.0f, 0.0f);
                }
            }

            currentMeasureRenderedCount++;
        }
        measurePosition += measure->measureWidth;
        measureNumber++;
        measureIndex++;
    } // measures loop
}

void App::RenderMultiMeasureRest(RenderData& renderData, unsigned int measureRestCount, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing)
{
    int spaces = lineCount - 1;
    float hBarYPosition = (lineSpacing * ((float)spaces/2.0f)) + measurePositionY; // the middle of staff

    float hBarSideMargin = 8.0f;
    float hBarPositionX = measurePositionX + hBarSideMargin;
    float hBarWidth = measureWidth - (hBarSideMargin * 2.0f);
    float hBarMidThickness = 8.0f;

    float hBarTotalHeight = 20.0f;
    float hBarOutsideLineThickness = 1.75f;

    Paint midLinePaint = Paint();
    midLinePaint.strokeWidth = hBarMidThickness;
    midLinePaint.strokeCap = Paint::Cap::Butt;

    // middle line
    renderData.AddLine(hBarPositionX,  hBarYPosition, hBarPositionX + hBarWidth,hBarYPosition, midLinePaint);

    Paint sideLinePaint = Paint();
    sideLinePaint.strokeWidth = hBarOutsideLineThickness;
    sideLinePaint.strokeCap = Paint::Cap::Butt;

    // side lines
    renderData.AddLine(hBarPositionX, hBarYPosition - (hBarTotalHeight/2.0f), hBarPositionX,hBarYPosition + (hBarTotalHeight/2.0f), sideLinePaint);
    renderData.AddLine(hBarPositionX + hBarWidth, hBarYPosition - (hBarTotalHeight/2.0f), hBarPositionX + hBarWidth,hBarYPosition + (hBarTotalHeight/2.0f), sideLinePaint);

    // render digits above

    std::string numberString = ToString(measureRestCount);
    float timeDigitWidth = 16.0f;
    float totalWidth = timeDigitWidth * (float)numberString.length();
    float startPositionX = ((measureWidth / 2.0f) - (totalWidth / 2.0f)) + measurePositionX;

    float positionY = measurePositionY - 20.0f;

    for (const char& c : numberString)
    {
        SMuFLID glyphID = GetTimeSignatureSMuFLID(ToInt(c));
        //LOGE("C: %c, value: %d, numMeasures: %d, string: %s", c, ToInt(c), measureRestCount, numberString.c_str());

        renderData.AddGlyph(SMuFLGlyph(glyphID, startPositionX, positionY, Paint(0xff000000)));

        startPositionX += timeDigitWidth;
    }
}

void App::RenderBarlines(RenderData& renderData, const std::vector<Barline>& barlines, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing)
{
    float measureHeight = (lineSpacing * float(lineCount - 1));
    float barlineLeftPositionX = measurePositionX;
    float barlineRightPositionX = measurePositionX + measureWidth;
    float barlinePositionYTop = measurePositionY;
    float barlinePositionYBottom = measurePositionY + measureHeight;

    float positionX = 0.0f;
    bool renderedRightBarline = false;

    for (const auto& barline : barlines)
    {
        if (barline.location == Barline::Location::Right)
            positionX = barlineRightPositionX, renderedRightBarline = true;
        else if (barline.location == Barline::Location::Left)
            positionX = barlineLeftPositionX;
        else if (barline.location == Barline::Location::Middle)
            LOGW("That barline location is not supported");

        RenderBarline(renderData, barline, positionX, barlinePositionYTop, measureHeight, lineCount, lineSpacing);
    }

    if (!renderedRightBarline)
        renderData.AddLine(std::make_shared<Line>(barlineRightPositionX, barlinePositionYTop, barlineRightPositionX, barlinePositionYBottom, BarLinePaint));

    // debug lines
    //renderData.AddLine(std::make_shared<Line>(barlineRightPositionX, barlinePositionYTop, barlineRightPositionX, barlinePositionYBottom, Paint(0xFFFF00FF)));
    //renderData.AddLine(std::make_shared<Line>(barlineLeftPositionX, barlinePositionYTop, barlineLeftPositionX, barlinePositionYBottom, Paint(0xFFFF00FF)));
}

void App::RenderBarline(RenderData& renderData, const Barline& barline, float barlinePositionX, float barlinePositionY, float barlineHeight, int lineCount, float lineSpacing)
{
    float barlinePositionYBottom = barlinePositionY + barlineHeight;

    float doubleBarlineOffset = 8.0f;
    float doubleBarlineOffsetDirection = 1.0f;

    // dots for repeat barlines

    if (barline.isRepeatBarLine)
    {
        float dotsPositionX = 0.0f;

        if (barline.facing == Barline::Direction::Forward)
            dotsPositionX = barlinePositionX + 15.0f;
        else if (barline.facing == Barline::Direction::Backward)
            dotsPositionX = barlinePositionX - 15.0f;

        if (barline.location == Barline::Location::Right)
            doubleBarlineOffsetDirection = -1.0f;
        else if (barline.location == Barline::Location::Left)
            doubleBarlineOffsetDirection = 0.0f;

        int spaces = lineCount - 1; // the number of spaces on the staff

        // the space that has the top most dot
        int topDotSpace = int(spaces / 2); // floor(spaces / 2)

        // the space that has the bottom most dot
        int bottomDotSpace = int((float)spaces / 2.0f + 1.5f); // floor

        float dotsPositionY = barlinePositionY + ((float)topDotSpace * lineSpacing) - (0.5f * lineSpacing); // barlineY + the spaces (scaled to lineSpcaing) - half a space to center the dot
        renderData.AddGlyph(SMuFLGlyph(SMuFLID::repeatDot, dotsPositionX, dotsPositionY, Paint(0xff000000)));

        // same thing but with bottomDotSpace
        dotsPositionY = barlinePositionY + ((float)bottomDotSpace * lineSpacing) - (0.5f * lineSpacing);
        renderData.AddGlyph(SMuFLGlyph(SMuFLID::repeatDot, dotsPositionX, dotsPositionY, Paint(0xff000000)));
    }

    // actual bar line

    if (barline.location == Barline::Location::Right)
        doubleBarlineOffsetDirection = -1.0f;
    else if (barline.location == Barline::Location::Left)
        doubleBarlineOffsetDirection = 0.0f;

    float barlineWidth = 8.0f;

    switch (barline.barlineStyle)
    {
        case Barline::BarlineStyle::Regular:
        {
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, BarLinePaint));
            break;
        }
        case Barline::BarlineStyle::LightLight:
        {
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, BarLinePaint));
            barlinePositionX += doubleBarlineOffset;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, BarLinePaint));
            break;
        }
        case Barline::BarlineStyle::HeavyLight:
        {
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, HeavyBarLinePaint));
            barlinePositionX += doubleBarlineOffset;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, BarLinePaint));
            break;
        }
        case Barline::BarlineStyle::LightHeavy:
        {
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, BarLinePaint));
            barlinePositionX += doubleBarlineOffset;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, HeavyBarLinePaint));
            break;
        }
        case Barline::BarlineStyle::HeavyHeavy:
        {
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, HeavyBarLinePaint));
            barlinePositionX += doubleBarlineOffset;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, HeavyBarLinePaint));
            break;
        }
        case Barline::BarlineStyle::NoBarline:
        default:
            break; // do nothing
    }
}

void App::RenderKeySignature(RenderData& renderData, const Measure* measure, float measurePosition, float ls, int lines, float offsetX, float offsetY)
{
    if (measure->showKeySignature && measure->keySignature.fifths != 0) {
        float positionX = measure->GetKeySignaturePositionInMeasure(song->GetMeasureWidth(measure->index)) + measurePosition + offsetX;
        float positionY = (ls * float(lines - 1)) + offsetY; // the bottom line + instYPosition

        int octaveOffset = 0;
        if (measure->clef.sign == "F")
            octaveOffset = -2;

        if (measure->keySignature.fifths > 0) // sharps
        {
            for (int n = 1; n <= measure->keySignature.fifths; n++) // loop through notes in 5ths ending at C#
            {
                float y = 0;
                switch (n)
                {
                    case 1: y = measure->GetPitchYPosition(Pitch("F", 5 + octaveOffset, 0.0f)); break;
                    case 2: y = measure->GetPitchYPosition(Pitch("C", 5 + octaveOffset, 0.0f)); break;
                    case 3: y = measure->GetPitchYPosition(Pitch("G", 5 + octaveOffset, 0.0f)); break;
                    case 4: y = measure->GetPitchYPosition(Pitch("D", 5 + octaveOffset, 0.0f)); break;
                    case 5: y = measure->GetPitchYPosition(Pitch("A", 4 + octaveOffset, 0.0f)); break;
                    case 6: y = measure->GetPitchYPosition(Pitch("E", 5 + octaveOffset, 0.0f)); break;
                    case 7: y = measure->GetPitchYPosition(Pitch("B", 4 + octaveOffset, 0.0f)); break;
                    default: break;
                }

                positionY = (ls * y) + offsetY;
                renderData.AddGlyph(SMuFLGlyph(SMuFLID::accidentalSharp, positionX, positionY, Paint(0xff000000)));
                positionX += 10.0f;
            }
        }
        else // flats
        {
            for (int n = -1; n >= measure->keySignature.fifths; n--) // loop through notes in descending 5ths ending at Cb
            {
                float y = 0.0f;
                switch (n)
                {
                    case -1: y = measure->GetPitchYPosition(Pitch("B", 4 + octaveOffset, 0.0f)); break;
                    case -2: y = measure->GetPitchYPosition(Pitch("E", 5 + octaveOffset, 0.0f)); break;
                    case -3: y = measure->GetPitchYPosition(Pitch("A", 4 + octaveOffset, 0.0f)); break;
                    case -4: y = measure->GetPitchYPosition(Pitch("D", 5 + octaveOffset, 0.0f)); break;
                    case -5: y = measure->GetPitchYPosition(Pitch("G", 4 + octaveOffset, 0.0f)); break;
                    case -6: y = measure->GetPitchYPosition(Pitch("C", 5 + octaveOffset, 0.0f)); break;
                    case -7: y = measure->GetPitchYPosition(Pitch("F", 4 + octaveOffset, 0.0f)); break;
                    default: break;
                }

                positionY = (ls * y) + offsetY;
                renderData.AddGlyph(SMuFLGlyph(SMuFLID::accidentalFlat, positionX, positionY, Paint(0xff000000)));
                positionX += 8.5f;
            }
        }
        // TODO: add naturals

        //renderData.AddBitmap(RenderBitmap(GetKeySignatureAssetID(measure->keySignature.fifths),positionX, positionY, 1.0f, 1.0f,Paint(0xff000000)));
    }
}

void App::RenderClef(RenderData& renderData, const Measure* measure, float measurePosition, float ls, int lines, float offsetX, float offsetY)
{
    // clef
    if (measure->showClef) {
        float positionX = measure->GetClefPositionInMeasure(song->GetMeasureWidth(measure->index)) + measurePosition + offsetX;
        float positionY = measure->GetClefLineYPosition(song->displayConstants, lines) + offsetY; // the bottom line + instYPosition
        renderData.AddGlyph(SMuFLGlyph(GetClefSMuFLID(measure->clef, lines), positionX, positionY, Paint(0xff000000)));
        //float positionX = measure->GetClefPositionInMeasure(song->GetMeasureWidth(measure->index)) + measurePosition + offsetX;
        //float positionY = (ls * float(lines - 1)) + offsetY; // the bottom line + instYPosition
        //renderData.AddBitmap(RenderBitmap(GetClefAssetID(measure->clef), positionX,positionY, 1.0f, 1.0f,Paint(0xff000000)));
    }
}

void App::RenderTimeSignature(RenderData& renderData, const Measure* measure, float measurePosition, float ls, int lines, float offsetX, float offsetY)
{
    // time signature
    if (measure->showTimeSignature and measure->timeSignature.print) {
        float positionX = measure->GetTimeSignaturePositionInMeasure(song->GetMeasureWidth(measure->index)) + measurePosition + offsetX;

        int spaces = lines - 1;
        float staffMiddle = ((float)spaces/2.0f) * ls;
        float timeDigitHeight = 10.0f;
        float positionY = (staffMiddle - timeDigitHeight) + offsetY;

        renderData.AddGlyph(
                SMuFLGlyph(GetTimeSignatureSMuFLID(measure->timeSignature.notes),positionX, positionY,Paint(0xff000000)));
        positionY += 20.0f;
        renderData.AddGlyph(
                SMuFLGlyph(GetTimeSignatureSMuFLID(measure->timeSignature.noteType),positionX, positionY,Paint(0xff000000)));
    }
}

void App::RenderLyric(RenderData& renderData, const Lyric& lyric, float notePositionX, float measurePositionY, float offsetX, float offsetY)
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (lyric.fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (lyric.fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = lyric.fontSize.size;

    //float positionX = song->GetPositionXInSong(note->beatPositionInSong, measure->index);
    float positionX = lyric.positionX + notePositionX;
    float positionY = lyric.positionY + measurePositionY;

    renderData.AddText(Text(lyric.text[0].text, positionX + offsetX, positionY + offsetY, Paint(lyric.color.color, paint)));
}

void App::RenderDirection(RenderData& renderData, const Direction& direction, float measurePositionY, Measure* measure, float measureXPosition, float offsetX, float offsetY)
{
    if (!direction.rehearsals.empty())
    {
        Rehearsal rehearsal = direction.rehearsals[0];
        Paint paint = Paint();
        paint.textSize = 16.0f;
        if (rehearsal.fontStyle == FontStyle::Italic)
            paint.isItalic = true;
        if (rehearsal.fontWeight == FontWeight::Bold)
            paint.isBold = true;
        paint.textSize = rehearsal.fontSize.size;
        //float positionX = song->GetPositionXInMeasure(direction.beatPositionInSong, measure->index) + measureXPosition;
        float positionX = rehearsal.positionX + measureXPosition;
        float positionY = rehearsal.positionY + measurePositionY;

        renderData.AddText(Text(rehearsal.text.string, positionX + offsetX, positionY + offsetY, Paint(rehearsal.color.color, paint)));
    }
    else if (!direction.words.empty())
    {
        Words words = direction.words[0];
        Paint paint = Paint();
        paint.textSize = 16.0f;
        if (words.fontStyle == FontStyle::Italic)
            paint.isItalic = true;
        if (words.fontWeight == FontWeight::Bold)
            paint.isBold = true;
        paint.textSize = words.fontSize.size;

        //float positionX = song->GetPositionXInSong(direction.beatPositionInSong, measure->index);
        float positionX = words.positionX + measureXPosition;
        float positionY = words.positionY + measurePositionY;

        renderData.AddText(Text(words.text.string, positionX + offsetX, positionY + offsetY, Paint(words.color.color, paint)));
    }
}

void App::RenderChord(RenderData& renderData, const Chord& chord, float measurePositionY, const Measure* measure, float measureXPosition, float offsetX, float offsetY)
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (chord.fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (chord.fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = chord.fontSize.size;
    //float positionX = song->GetPositionXInMeasure(chord.beatPositionInSong, measure->index) + measureXPosition;
    float positionX = chord.positionX + measureXPosition;
    float positionY = chord.positionY + measurePositionY;
    //LOGE("Chord (%s) is at %f, %f", chord.chordName.string.c_str(), chord.beatPosition, positionX);
    renderData.AddText(Text(chord.chordName.string, positionX + offsetX, positionY + offsetY, Paint(chord.color.color, paint)));
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

void App::RenderRest(RenderData& renderData, const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY)
{
    // calculate color of the note
    int color = normalColor;
    if (note->isPlaying) {
        color = playedColor;
    }

    // rendering rest

    //float positionX = song->GetPositionXInMeasure(note->beatPositionInSong, note->measureIndex) + measurePositionX + offsetX;
    //float positionY = ((ls * float(lines - 1)) / 2.0f) + offsetY;

    //float positionX = note->defaultX + measurePositionX;
    //float positionY = note->defaultY + staffPositionY;

    float positionX = note->positionX + measurePositionX + offsetX;

    int line = (lines / 2) + 1;
    float linePositionY = note->positionY + (float(line-1) * ls) + offsetY; // the position of the line that all rests "hang" on (except for the whole rest)
    float positionY = linePositionY;

    if (note->isFullMeasureRest) {
        positionY -= ls;
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(note->durationType), positionX, positionY, Paint(color)));
    }
    else if (note->durationType == Note::NoteDurationType::Whole) {
        positionY -= ls;
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(note->durationType), positionX, positionY, Paint(color)));
    }
    else {
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(note->durationType), positionX, positionY, Paint(color)));
    }
}

void App::RenderTabNote(RenderData& renderData, const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY)
{
    // calculate color of the note
    int color = normalColor;
    if (note->isPlaying) {
        color = playedColor;
    }

    //float positionX = song->GetPositionXInMeasure(note->beatPositionInSong,note->measureIndex) + measurePositionX + offsetX; // this line of code crashes the program
    //float positionY = (ls * float(note->string - 1)) + offsetY;

    float positionX = note->positionX + measurePositionX + offsetX;
    float positionY = note->positionY + offsetY;

    renderData.AddText(
            Text(ToString(note->fret), positionX, positionY,
                 Paint(color, TabTextPaint)));

    // hammer-ons and pull-offs
    for (const TABSlur& slur : note->tabSlurs) {
        if (slur.type == StartStopType::Stop) {
            CubicCurve curve = CubicCurve();

            // start
            curve.x1 = curveStartX;
            curve.y1 = curveStartY;

            // curve points
            curve.x2 = curveStartX + 10.0f;
            curve.y2 = curveStartY - 10.0f;

            curve.x3 = positionX - 10.0f;
            curve.y3 = curveStartY - 10.0f;

            // end
            curve.x4 = positionX;
            curve.y4 = positionY - 6.0f;

            curve.paint = TabSlurPaint;

            renderData.AddCubicCurve(curve);
        }

        if (slur.type == StartStopType::Start) {
            curveStartX = positionX;
            curveStartY = positionY - 6.0f;
        }
    }
}

void App::RenderNote(RenderData& renderData, const Note* note, Measure* measure, float measurePositionX, const Staff* staff, float measurePositionY, int measureNumber, float ls, float mainPositionX, float mainPositionY, int noteIndex)
{
    // calculate color of the note
    int color = normalColor;
    if (note->isPlaying) {
        color = playedColor;
    }

    if (note->isRest) { // is a rest
        RenderRest(renderData, note, measurePositionX, staff->lines, ls, mainPositionX, mainPositionY + measurePositionY);
    } else if (note->type == Note::NoteType::Tab) // is a tab note
    {
        RenderTabNote(renderData, note, measurePositionX, staff->lines, ls, mainPositionX, mainPositionY + measurePositionY);
    } else // is a standard note
    {
        LOGV("Rendering a standard note: %d", (int)note->type);
        // rendering note head
        //float positionX = measure->GetNotePositionInMeasure(
        //        song->GetMeasureWidth(measureNumber), noteIndex) + measurePosition;

        //float positionX = song->GetPositionXInMeasure(note->beatPositionInSong, measureNumber) + measurePositionX;
        //float positionY = (ls * measure->GetNoteYPosition(noteIndex)) + staffYPosition + instYPosition;

        float positionX = note->positionX + measurePositionX;
        float positionY = note->positionY + measurePositionY;

        //float positionX = note->defaultX + measurePositionX;
        //float positionY = note->defaultY + staffYPosition + instYPosition;

        renderData.AddGlyph(SMuFLGlyph(GetNoteHeadSMuFLID(note->durationType),positionX + mainPositionX, positionY + mainPositionY, Paint(color)));

        // aug dot
        for (const auto& dot : note->dots)
        {
            renderData.AddGlyph(SMuFLGlyph(SMuFLID::augmentationDot, dot.positionX + positionX + mainPositionX, dot.positionY + positionY + mainPositionY, Paint(dot.color.color)));
        }

        // ledger lines
        if (measure->CalculateNoteYPositionRelativeToMeasure(noteIndex) >=
            (float) staff->lines) // ledger lines below staff
        {
            int ledgerLineCount =
                    (int) measure->CalculateNoteYPositionRelativeToMeasure(noteIndex) -
                    staff->lines + 1;
            float y = measurePositionY +
                      ((float) staff->lines * ls);
            for (int i = 0; i < ledgerLineCount; i++) {
                renderData.AddLine(std::make_shared<Line>(positionX - 4.0f + mainPositionX, y + mainPositionY, positionX + 14.0f + mainPositionX, y + mainPositionY,
                             BarLinePaint));
                y += 1.0f * ls;
            }
        }
        if (measure->CalculateNoteYPositionRelativeToMeasure(noteIndex) <
            0.0f) // ledger lines above staff
        {
            int ledgerLineCount = abs(
                    (int) measure->CalculateNoteYPositionRelativeToMeasure(noteIndex));
            float y = measurePositionY - (1.0f * ls);
            for (int i = 0; i < ledgerLineCount; i++) {
                renderData.AddLine(std::make_shared<Line>(positionX - 4.0f + mainPositionX, y + mainPositionY, positionX + 14.0f + mainPositionX, y + mainPositionY,
                             BarLinePaint));
                y -= 1.0f * ls;
            }
        }

        for (Slur slur: note->slurs) {
            if (slur.type == StartStopType::Stop) {
                CubicCurve curve = CubicCurve();

                // start
                curve.x1 = curveStartX;
                curve.y1 = curveStartY;

                // curve points
                curve.x2 = curveStartX + 10.0f;
                curve.y2 = curveStartY - 10.0f;

                curve.x3 = positionX - 10.0f;
                curve.y3 = curveStartY - 10.0f;

                // end
                curve.x4 = positionX;
                curve.y4 = positionY - 6.0f;

                curve.paint = TabSlurPaint;

                renderData.AddCubicCurve(curve);
            }

            if (slur.type == StartStopType::Start) {
                curveStartX = positionX;
                curveStartY = positionY - 6.0f;
            }
        }

        RenderNoteStemAndFlagAndBeam(renderData, note, positionX + mainPositionX, positionY + mainPositionY);

        if (note->accidental.accidentalType !=
            Accidental::AccidentalType::None) {
            float accX = positionX - 10.0f;
            float accY = positionY;

            renderData.AddGlyph(SMuFLGlyph(GetAccidentalSMuFLID(note->accidental.accidentalType),accX + mainPositionX, accY + mainPositionY, Paint(0xff000000)));
        }
    }

    for (const auto &lyric: note->lyrics) {
        /*float positionY = (ls * (float) staff->lines) +
                          staffYPosition +
                          instYPosition + 20.0f;*/
        RenderLyric(renderData, lyric, note->positionX + measurePositionX, measurePositionY, mainPositionX, mainPositionY);
    } // lyrics loop
}

void App::RenderNoteStemAndFlagAndBeam(RenderData& renderData, const Note* note, float notePositionX, float notePositionY)
{
    // rendering note stem
    float noteWidth = 11.3f;
    float stemEndY = 0.0f;
    float stemPositionX = 0.0f;

    float stemLength = 30.0f;
    if (note->noteStem.stemType == NoteStem::StemType::Up) {
        stemPositionX = notePositionX + noteWidth - NoteStemPaint.strokeWidth / 2.0f;
        renderData.AddLine(std::make_shared<Line>(stemPositionX, notePositionY, stemPositionX, notePositionY - stemLength, NoteStemPaint));
        stemEndY = notePositionY - stemLength;
    } else if (note->noteStem.stemType == NoteStem::StemType::Down) {
        stemPositionX = notePositionX + NoteStemPaint.strokeWidth / 2.0f;
        renderData.AddLine(std::make_shared<Line>(stemPositionX, notePositionY, stemPositionX, notePositionY + stemLength, NoteStemPaint));
        stemEndY = notePositionY + stemLength;
    } else if (note->noteStem.stemType == NoteStem::StemType::Double) {
        stemPositionX = notePositionX + NoteStemPaint.strokeWidth / 2.0f;
        renderData.AddLine(std::make_shared<Line>(stemPositionX, notePositionY, stemPositionX, notePositionY + stemLength,NoteStemPaint));
        stemPositionX = notePositionX + noteWidth - NoteStemPaint.strokeWidth / 2.0f;
        renderData.AddLine(std::make_shared<Line>(stemPositionX, notePositionY, stemPositionX, notePositionY - stemLength, NoteStemPaint));
    }

    // rendering note beam
    if (note->beam.beamType == Beam::BeamType::Begin) {
        if (note->noteStem.stemType == NoteStem::StemType::Up) {
            beamStartX = notePositionX + noteWidth;
            beamStartY = stemEndY - (float)(note->beam.beamLevel-1)*10.0f;
        } else if (note->noteStem.stemType == NoteStem::StemType::Down) {
            beamStartX = notePositionX;
            beamStartY = stemEndY + (float)(note->beam.beamLevel-1)*10.0f;
        }
    } else if (note->beam.beamType == Beam::BeamType::Continue) {
        // do something
    } else if (note->beam.beamType == Beam::BeamType::End) {
        float beamEndX = 0.0f;
        float beamEndY = 0.0f;

        if (note->noteStem.stemType == NoteStem::StemType::Up) {
            beamEndX = notePositionX + noteWidth;
            beamEndY = stemEndY + (float)(note->beam.beamLevel-1)*10.0f;
        } else if (note->noteStem.stemType == NoteStem::StemType::Down) {
            beamEndX = notePositionX;
            beamEndY = stemEndY - (float)(note->beam.beamLevel-1)*10.0f;
        }

        renderData.AddLine(std::make_shared<Line>(beamStartX, beamStartY, beamEndX,beamEndY,NoteBeamPaint));
    }

    // note flag
    if (note->beam.beamType == Beam::BeamType::None)
    {
        bool isUp = false;

        if (note->noteStem.stemType == NoteStem::StemType::Up) {
            isUp = true;
        } else if (note->noteStem.stemType == NoteStem::StemType::Down) {
            isUp = false;
        }

        renderData.AddGlyph(SMuFLGlyph(GetNoteFlagSMuFLID(note->durationType, isUp), stemPositionX, stemEndY, Paint(normalColor)));
    }
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
