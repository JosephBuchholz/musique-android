#include "MusicRenderer.h"

#include "Utils/Converters.h"
#include "RenderMeasurement.h"

#include "Callbacks.h"

#include "MusicData/ComplexLine.h"

MusicRenderer::MusicRenderer()
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

void MusicRenderer::Render(std::shared_ptr<Song> song, Settings settings)
{
    switch (settings.musicLayout)
    {
        case Settings::MusicLayout::Horizontal: {
            //RenderHorizontalLayout();
            break;
        }
        case Settings::MusicLayout::Vertical: {
            /*if (!layoutCalculated)
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

            UpdateFrameData(frameData);*/
            break;
        }
        case Settings::MusicLayout::Paged: {
            if (!layoutCalculated)
                CalculateRenderForPagedLayout(song, settings);

            RenderWithRenderData();

            /*int currentSystemIndex = song->GetSystemIndex(currentMeasure);
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

            UpdateFrameData(frameData);*/
            break;
        }
        default: {
            //LOGE("This layout is not supported");
            break;
        }
    }
}

void MusicRenderer::CalculateRenderForPagedLayout(std::shared_ptr<Song> song, Settings settings)
{
    LOGI("Calculating Paged Layout");
    m_RenderData = RenderData();

    pagePositions.clear();
    systemPositions.clear();

    int totalPages = OnCalculateNumPages(song);
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
        RenderMusicToPage(song, pageIndex, m_RenderData, pageX, pageY);

        pagePositions.emplace_back(pageX, pageY);
        pageIndex++;

        // debug page lines
        m_RenderData.AddLine(std::make_shared<Line>(pageX, pageY, pageX + song->displayConstants.pageWidth, pageY, BarLinePaint));
        m_RenderData.AddLine(std::make_shared<Line>(pageX, pageY, pageX, pageY + song->displayConstants.pageHeight, BarLinePaint));

        m_RenderData.AddLine(std::make_shared<Line>(pageX, pageY + song->displayConstants.pageHeight, pageX + song->displayConstants.pageWidth, pageY + song->displayConstants.pageHeight, BarLinePaint));
        m_RenderData.AddLine(std::make_shared<Line>(pageX + song->displayConstants.pageWidth, pageY, pageX + song->displayConstants.pageWidth, pageY + song->displayConstants.pageHeight, BarLinePaint));
    }

#if SHOW_BOUNDING_BOXES

    //song->UpdateBoundingBoxes(pagePositions, systemPositions, m_RenderData);
    song->RenderBoundingBoxes(m_RenderData, pagePositions, systemPositions);
#endif

    layoutCalculated = true;
}

void MusicRenderer::RenderMusicToPage(std::shared_ptr<Song> song, int page, RenderData& pageRenderData, float pageX, float pageY) {

    // --- MAIN RENDERING ---


    LOGI("Rendering pages for printing");

    /*std::shared_ptr<ComplexLine> wavyLine = std::make_shared<ComplexLine>(ComplexLine::ComplexLineType::WavyTrill);
    Vec2<float> pos = { pageX + 10.0f + pageRenderData.displayConstants.pageWidth, pageY + 10.0f };

    wavyLine->positionStart = { 0.0f, 0.0f };
    wavyLine->positionEnd = { 500.0f, 1.0f };

    wavyLine->complexLineType = ComplexLine::ComplexLineType::WavyTrill;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 20.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::WavyVibrato;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 20.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::WavyVibratoWide;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 20.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::WavyGuitarVibrato;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 20.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::WavyGuitarWideVibrato;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 20.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::Wavy;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 20.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::Sawtooth;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 50.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::Squaretooth;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 50.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::Circular;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 50.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::AccelRitBeam;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 50.0f;

    wavyLine->complexLineType = ComplexLine::ComplexLineType::Random;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 100.0f;

    wavyLine = std::make_shared<ComplexLine>(ComplexLine::ComplexLineType::ArpeggioUp);
    wavyLine->endGlyph = SMuFLID::wiggleArpeggiatoUpArrow;
    wavyLine->positionStart = pos;
    wavyLine->positionEnd = { pos.x + 1.0f, pos.y + 1.0f };
    wavyLine->Render(pageRenderData, pos);
    wavyLine->positionEnd = { pos.x - 20.0f, pos.y + 20.0f };
    wavyLine->Render(pageRenderData, pos);
    wavyLine->positionEnd = { pos.x + 1.0f, pos.y - 1.0f };
    wavyLine->Render(pageRenderData, pos);
    wavyLine->positionEnd = { pos.x - 1.0f, pos.y - 1.0f };
    wavyLine->Render(pageRenderData, pos);
    wavyLine->positionEnd = { pos.x + 1.0f, pos.y };
    wavyLine->Render(pageRenderData, pos);
    wavyLine->positionEnd = { pos.x, pos.y + 1.0f };
    wavyLine->Render(pageRenderData, pos);
    wavyLine->positionEnd = { pos.x - 1.0f, pos.y };
    wavyLine->Render(pageRenderData, pos);
    wavyLine->positionEnd = { pos.x, pos.y - 1.0f };
    wavyLine->Render(pageRenderData, pos);
    wavyLine->positionEnd = { pos.x + 30.0f, pos.y - 3.0f };
    wavyLine->Render(pageRenderData, pos);

    pos.y += 20.0f;*/

    /*wavyLine = std::make_shared<ComplexLine>(ComplexLine::ComplexLineType::ArpeggioDown);
    wavyLine->startGlyph = SMuFLID::wiggleArpeggiatoDownArrow;
    wavyLine->Render(pageRenderData, pos);
    pos.y += 20.0f;*/

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

        bool drawFullInstNames = true;

        Vec2<float> systemPosition = { pageX + song->displayConstants.leftMargin + song->systems[systemIndex].layout.systemLeftMargin,
                                       pageY + song->displayConstants.topMargin + song->systems[systemIndex].layout.topSystemDistance};

        if (page == 0)
        {
            RenderCredits(pageRenderData, song, song->displayConstants, song->credits, pageX, pageY);
        }

        if (page < song->pageNumbers.size())
            song->pageNumbers[page].Render(pageRenderData, { pageX, pageY });

        for (int i = startingMeasureIndex; i <= measureCount; i++)
        {
            bool startNewSystem;

            if (i == startingMeasureIndex) // first time
            {
                startNewSystem = false;
            }
            else if (i < measureCount) // any other valid measure
            {
                startNewSystem = song->DoesMeasureStartNewSystem(i);
            }
            else
            {
                startNewSystem = true;
            }

            if ((startNewSystem || i == measureCount) && i != 0) { // render system
                startMeasure = endMeasure+1;
                endMeasure = i-1;

                systemPosition = RenderSystem(pageRenderData, song, startMeasure, endMeasure, systemIndex, systemPosition, { pageX, pageY }, drawFullInstNames);

                drawFullInstNames = false;

                startMeasure = i;
                systemIndex++;
            } // system loop (sort of)

            if (song->GetPageIndex(i) > page) // don't render the next page
            {
                break;
            }

            if (systemIndex >= systemPositions.size())
                systemPositions.push_back(systemPosition);
        }

        for (auto ending : song->endings)
        {
            Vec2<float> systemPosition = { 0.0f, 0.0f };
            int systemIndex = song->GetSystemIndex(ending->startMeasureIndex);
            if (systemIndex < systemPositions.size())
            {
                systemPosition.x = systemPositions[systemIndex].x;
                systemPosition.y = systemPositions[systemIndex].y;
            }

            Vec2<float> measureStartPosition = { systemPosition.x + song->GetMeasurePositionX(ending->startMeasureIndex), systemPosition.y };
            Vec2<float> measureEndPosition = { systemPosition.x + song->GetMeasurePositionX(ending->endMeasureIndex - 1) + song->GetMeasureWidth(ending->endMeasureIndex - 1), systemPosition.y };
            if (ending)
                ending->Render(pageRenderData, measureStartPosition, measureEndPosition);
        }
    }
}

Vec2<float> MusicRenderer::RenderSystem(RenderData& renderData, std::shared_ptr<Song> song, unsigned int startMeasure, unsigned int endMeasure, int systemIndex, Vec2<float> systemPosition, Vec2<float> pagePosition, bool drawFullInstNames)
{
    std::shared_ptr<Instrument> prevInstrument = nullptr;
    int instrumentIndex = 0;
    float instYPosition = systemPosition.y;
    for (auto instrument: song->instruments) {

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
                if (drawFullInstNames)
                {
                    renderData.AddText(Text(instrument->name.string, systemPosition.x - 10.0f, textPositionY, InstNameTextPaint));
                }
                else
                {
                    renderData.AddText(Text(instrument->nameAbbreviation.string, systemPosition.x - 10.0f, textPositionY, InstNameTextPaint));
                }
            }

            int staffIndex = 0;
            for (auto staff : instrument->staves) {
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

                RenderLineOfMeasures(renderData, song, startMeasure, endMeasure, song->systems[systemIndex], staff, systemPosition.x, staffYPosition + instYPosition, ls, instrumentIndex == 0 && staffIndex == 0);

                staffIndex++;
            } // staves loop

            prevInstrument = instrument;
        } // instrument is visible

        instrumentIndex++;
    } // instruments loop

    float lastInstrumentMiddleHeight = prevInstrument->GetMiddleHeight(10.0f, 13.33f, startMeasure, endMeasure);

    if (song->instruments.size() != 1) // more than one instrument
    {
        // system connecting line
        renderData.AddLine(std::make_shared<Line>(systemPosition.x, systemPosition.y, systemPosition.x, instYPosition + lastInstrumentMiddleHeight, BarLinePaint));
    }

    if (prevInstrument != nullptr)
        systemPosition.y = instYPosition + song->systems[systemIndex + 1].layout.systemDistance + lastInstrumentMiddleHeight;
    systemPosition.x = pagePosition.x + song->displayConstants.leftMargin + song->systems[systemIndex + 1].layout.systemLeftMargin;

    return systemPosition;
}

void MusicRenderer::RenderWithRenderData()
{
    if (updateRenderData)
    {
        UpdateRender(m_RenderData);
        updateRenderData = false;
    }
}

void MusicRenderer::RenderLineOfMeasures(RenderData& renderData, std::shared_ptr<Song> song, unsigned int startMeasure, unsigned int endMeasure, const System& system, std::shared_ptr<Staff> staff, float systemPositionX, float staffPositionY, float lineSpacing, bool isTopMeasureLine)
{
    bool multiMeasureRest = false; // whether the measure is part of a multi measure rest
    unsigned int numberOfMeasuresInMultiMeasureRest = 0; // number of measures left in multi measure rest
    unsigned int measureThatStartedMultiMeasureRest = 0; // the index of the measure that started the multi measure rest

    LOGD("startMeasure: %d, endMeasure: %d", startMeasure, endMeasure);

    int measureIndex = (int)startMeasure;
    int measureNumber = (int)startMeasure;
    float measurePositionX = systemPositionX;
    float nextMeasurePositionX = 0.0f;
    int measureRenderCount = 15; // the number of measures that need to be rendered
    int currentMeasureRenderedCount = 0; // the number of measures that have been rendered
    bool isLastMeasureInSystem = false;
    for (int m = (int)startMeasure; m <= endMeasure; m++) {

        std::shared_ptr<Measure> measure = staff->measures[m];

        nextMeasurePositionX = measurePositionX + measure->measureWidth;

        if (m == endMeasure)
            isLastMeasureInSystem = true;

        //if (m == currentMeasure)
        //{
        //    playLinePosition = song->GetPositionXInMeasure(playLineBeatPosition, currentMeasure) + measurePositionX;
        //}

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

        int currentMeasure = 0; // TODO: remove

        if (!(currentMeasureRenderedCount >= measureRenderCount) &&
            measureNumber >= currentMeasure - (measureRenderCount / 2) &&
            !multiMeasureRest) // render measure in a radius of measureRenderCount/2
        {
            if (measure->startsMultiMeasureRest)
            {
                //measure->measureWidth = 80.0f;
                multiMeasureRest = true;
                numberOfMeasuresInMultiMeasureRest = measure->numberOfMeasuresInMultiMeasureRest;
                measureThatStartedMultiMeasureRest = measureIndex;

                RenderMultiMeasureRest(renderData, measure->numberOfMeasuresInMultiMeasureRest, measurePositionX, staffPositionY, measure->measureWidth, staff->lines, lineSpacing);
            }

            // staff lines
            for (int j = 0; j < staff->lines; j++) {
                float endX = measurePositionX + measure->measureWidth;
                std::shared_ptr<Line> line = renderData.AddLine(measurePositionX, (lineSpacing * (float)j) + staffPositionY, endX,(lineSpacing * (float)j) + staffPositionY,BarLinePaint);
                //renderableSong.systems[systemIndex].instruments[instrumentIndex].staves[staffIndex].measures[measureIndex].staffLines.push_back(std::make_shared<Line>(0.0f, 0.0f, 0.0f, 1.0f));
            }

            //LOGV("barline count: %d, measure index: %d", measure->barlines.size(), measure->index);
            RenderBarlines(renderData, measure->barlines, measurePositionX, staffPositionY, measure->measureWidth, staff->lines, lineSpacing);
            // measure lines (bar lines)
            float x = measurePositionX;
            if (!(startMeasure == m && song->instruments.size() == 1)) // !(it is the first measure and only one instrument)
            {
                renderData.AddLine(std::make_shared<Line>(x, 0.0f + staffPositionY, x, (lineSpacing * float(staff->lines - 1)) + staffPositionY, BarLinePaint));
            }
            x += measure->measureWidth;
            renderData.AddLine(std::make_shared<Line>(x, 0.0f + staffPositionY, x,(lineSpacing * float(staff->lines - 1)) + staffPositionY, BarLinePaint));

            auto measureDataItem = system.systemMeasureData.find(measure->index);

            if (measureDataItem != system.systemMeasureData.end())
            {
                bool showTimeSignature = measure->showTimeSignature || (system.showBeginningTimeSignature && song->DoesMeasureStartNewSystem(measure->index));
                measure->timeSignature.Render(renderData, showTimeSignature, measureDataItem->second.timeSignaturePositionX + measurePositionX, staffPositionY, lineSpacing, staff->lines);

                bool showKeySignature = (measure->showKeySignature || (system.showBeginningKeySignature && song->DoesMeasureStartNewSystem(measure->index)));
                measure->keySignature.Render(renderData, showKeySignature, measureDataItem->second.keySignaturePositionX + measurePositionX, lineSpacing, staff->lines, measure->clef, 0.0f, staffPositionY);

                bool showSystemClef = (system.showBeginningClef && song->DoesMeasureStartNewSystem(measure->index));

                float clefPositionX;
                if (measure->clef.clefChanged)
                {
                    clefPositionX = measurePositionX - 28.0f;
                }
                else
                {
                    clefPositionX = measureDataItem->second.clefPositionX + measurePositionX;
                }

                measure->clef.Render(renderData, showSystemClef, clefPositionX, song->displayConstants, staff->lines, 0.0f, staffPositionY);
            }

            if (startMeasure == m && isTopMeasureLine && measure->measureNumber.GetNumber() != 1)
            {
                measure->measureNumber.Render(renderData, {measurePositionX, staffPositionY });
            }

            // render directions
            for (const Direction &direction: measure->directions) {
                float measurePositionY = staffPositionY;

                direction.Render(renderData, { measurePositionX, measurePositionY });
            }

            if (!measure->startsMultiMeasureRest)
            {
                int noteIndex = 0;
                for (auto note : measure->notes) {
                    note->Render(renderData, staff->tablatureDisplayType, measure->CalculateNoteYPositionRelativeToMeasure(noteIndex), staff->lines, { measurePositionX, staffPositionY }, nextMeasurePositionX, measure->measureWidth, measureNumber, lineSpacing, { 0.0f, 0.0f }, noteIndex, isLastMeasureInSystem);
                    noteIndex++;
                }

                // rendering note beams
                for (const BeamGroup& beamGroup : measure->beams)
                {
                    for (const Beam& beam : beamGroup.beams)
                    {
                        if (beamGroup.notes.empty())
                            break;

                        if (beamGroup.notes[0] == nullptr)
                        {
                            LOGE("NOTE[0] IS NULL POINTER!!!!!");
                            break;
                        }

                        float size;
                        if (beamGroup.notes[0]->noteSize == NoteSize::Grace)
                            size = renderData.displayConstants.graceNoteSize;
                        else if (beamGroup.notes[0]->noteSize == NoteSize::Cue)
                            size = renderData.displayConstants.cueNoteSize;
                        else
                            size = 1.0f;

                        const float beamDistance = 8.0f * size;
                        const float hookOffset = 10.0f * size;
                        float beamYOffset = (beamDistance * float(beam.beamLevel-1));

                        Paint paint = NoteBeamPaint;
                        paint.strokeWidth *= size;

                        if (beamGroup.isAboveNote)
                        {
                            beamYOffset = -beamYOffset;
                        }

                        if (beam.beamType == Beam::BeamType::Normal)
                        {
                            renderData.AddLine(std::make_shared<Line>(beam.beamStartPositionX + measurePositionX, beam.beamStartPositionY + staffPositionY - beamYOffset, beam.beamEndPositionX + measurePositionX, beam.beamEndPositionY + staffPositionY - beamYOffset, paint));
                        }
                        else if (beam.beamType == Beam::BeamType::ForwardHook)
                        {
                            renderData.AddLine(std::make_shared<Line>(beam.beamStartPositionX + measurePositionX, beam.beamStartPositionY + staffPositionY - beamYOffset, beam.beamStartPositionX + measurePositionX + hookOffset, beamGroup.GetPositionYOnBeam(beam.beamStartPositionX + hookOffset) + staffPositionY - beamYOffset, paint));
                        }
                        else if (beam.beamType == Beam::BeamType::BackwardHook)
                        {
                            renderData.AddLine(std::make_shared<Line>(beam.beamStartPositionX + measurePositionX, beam.beamStartPositionY + staffPositionY - beamYOffset, beam.beamStartPositionX + measurePositionX - hookOffset, beamGroup.GetPositionYOnBeam(beam.beamStartPositionX - hookOffset) + staffPositionY - beamYOffset, paint));
                        }
                    }
                }

                // render all chords in this measure
                for (Chord &chord : measure->chords) {
                    chord.CalculateChordName();
                    float measurePositionY = staffPositionY;
                    chord.Render(renderData, measurePositionX, measurePositionY);
                }

                for (auto tuplet : measure->tuplets)
                {
                    float measurePositionY = staffPositionY;
                    tuplet->Render(renderData, Vec2<float>(measurePositionX, measurePositionY));
                }

                for (auto arpeggio : measure->arpeggios)
                {
                    float measurePositionY = staffPositionY;
                    float notePositionX = measurePositionX;
                    if (arpeggio)
                        arpeggio->Render(renderData, notePositionX, measurePositionY);
                }
            }

            currentMeasureRenderedCount++;
        }
        measurePositionX = nextMeasurePositionX;
        measureNumber++;
        measureIndex++;
    } // measures loop
}

void MusicRenderer::RenderMultiMeasureRest(RenderData& renderData, unsigned int measureRestCount, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing)
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

void MusicRenderer::RenderBarlines(RenderData& renderData, const std::vector<Barline>& barlines, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing)
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

void MusicRenderer::RenderBarline(RenderData& renderData, const Barline& barline, float barlinePositionX, float barlinePositionY, float barlineHeight, int lineCount, float lineSpacing)
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

    Paint paint = BarLinePaint;
    barline.VisibleElement::ModifyPaint(paint);
    barline.LineElement::ModifyPaint(paint);

    switch (barline.barlineStyle)
    {
        case Barline::BarlineStyle::Regular:
        {
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, paint));
            break;
        }
        case Barline::BarlineStyle::LightLight:
        {
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, paint));
            barlinePositionX += doubleBarlineOffset;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, paint));
            break;
        }
        case Barline::BarlineStyle::HeavyLight:
        {
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, HeavyBarLinePaint));
            barlinePositionX += doubleBarlineOffset;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, paint));
            break;
        }
        case Barline::BarlineStyle::LightHeavy:
        {
            barlinePositionX += barlineWidth * doubleBarlineOffsetDirection;
            renderData.AddLine(std::make_shared<Line>(barlinePositionX, barlinePositionY, barlinePositionX, barlinePositionYBottom, paint));
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

void MusicRenderer::RenderCredits(RenderData& renderData, std::shared_ptr<Song> song, const MusicDisplayConstants& displayConstants, const std::vector<Credit>& credits, float pageX, float pageY)
{
    for (const Credit& credit : credits)
    {
        credit.Render(renderData, { pageX, pageY }, { song->displayConstants.pageWidth, song->displayConstants.pageHeight });
    }
}

int MusicRenderer::OnCalculateNumPages(std::shared_ptr<Song> song)
{
    int numPages = song->GetNumPages();

    if (numPages == 0)
        LOGE("NUM PAGES IS 0");

    LOGI("There are %d pages", numPages);

    return numPages;
}