#include "MusicRenderer.h"

#include "Utils/Converters.h"
#include "RenderMeasurement.h"

#include "Callbacks.h"

#include "MusicData/ComplexLine.h"
#include "Utils/Math.h"

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

void MusicRenderer::Render(const std::shared_ptr<Song>& song, const Settings& settings)
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

            m_RenderData.left = 0.0f;
            m_RenderData.right = (song->displayConstants.pageWidth + pageGap) * (float)song->GetNumPages() + 10.0f;
            m_RenderData.top = 0.0f;
            m_RenderData.bottom = song->displayConstants.pageHeight + 10.0f;

            RenderWithRenderData();
            break;
        }
        default: {
            //LOGE("This layout is not supported");
            break;
        }
    }
}

void MusicRenderer::CalculateRenderForPagedLayout(const std::shared_ptr<Song>& song, const Settings& settings)
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
                pageY += song->displayConstants.pageHeight + pageGap;
            else if (settings.pagesOrientation == Orientation::Horizontal)
                pageX += song->displayConstants.pageWidth + pageGap;
        }

        // ---- RENDER ----
        RenderMusicToPage(song, pageIndex, m_RenderData, settings, { pageX, pageY });
#if SHOW_BOUNDING_BOXES
        RenderDebugToPage(song, pageIndex, m_RenderData, settings, pageX, pageY);
#endif

        pagePositions.emplace_back(pageX, pageY);
        pageIndex++;

        // debug page lines
        m_RenderData.AddLine(Line({ pageX, pageY }, { pageX + song->displayConstants.pageWidth, pageY }, BarLinePaint));
        m_RenderData.AddLine(Line({ pageX, pageY }, { pageX, pageY + song->displayConstants.pageHeight }, BarLinePaint));

        m_RenderData.AddLine(Line({ pageX, pageY + song->displayConstants.pageHeight }, { pageX + song->displayConstants.pageWidth, pageY + song->displayConstants.pageHeight }, BarLinePaint));
        m_RenderData.AddLine(Line({ pageX + song->displayConstants.pageWidth, pageY }, { pageX + song->displayConstants.pageWidth, pageY + song->displayConstants.pageHeight }, BarLinePaint));
    }

#if SHOW_BOUNDING_BOXES

    //song->UpdateBoundingBoxes(pagePositions, systemPositions, m_RenderData);
    song->RenderBoundingBoxes(m_RenderData, pagePositions, systemPositions);
#endif

    layoutCalculated = true;
}

void MusicRenderer::RenderMusicToPage(const std::shared_ptr<Song>& song, int page, RenderData& pageRenderData, const Settings& settings, Vec2<float> pagePosition)
{

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

    if (updateRenderData)
    {

        int measureCount = song->GetMeasureCount();

        int startingMeasureIndex = song->GetFirstMeasureOnPage(page);

        int startMeasure = startingMeasureIndex;
        int endMeasure = startingMeasureIndex-1;

        int systemIndex = song->GetSystemIndex(startingMeasureIndex);

        LOGD("page: %d, start measure: %d, systemIndex: %d, pages: %d", page, startingMeasureIndex, systemIndex, song->GetNumPages());

        bool drawFullInstNames = true;

        Vec2<float> systemPosition = { pagePosition.x + song->displayConstants.leftMargin + song->systems[systemIndex]->layout.systemLeftMargin,
                                       pagePosition.y + song->displayConstants.topMargin + song->systems[systemIndex]->layout.topSystemDistance};

        systemPosition.x = pagePosition.x + song->systems[systemIndex]->position.x;
        systemPosition.y = pagePosition.y + song->systems[systemIndex]->position.y;

        if (page == 0)
        {
            RenderCredits(pageRenderData, song, song->displayConstants, song->credits, pagePosition.x, pagePosition.y);
        }

        if (page < song->GetNumPages())
            song->pages[page].pageNumber.Render(pageRenderData, pagePosition);

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

            if ((startNewSystem || i == measureCount) && i != 0 && systemIndex < song->systems.size()) // render system
            {
                startMeasure = endMeasure+1;
                endMeasure = i-1;

                systemPosition = RenderSystem(pageRenderData, settings, song, startMeasure, endMeasure, systemIndex, systemPosition, pagePosition, drawFullInstNames);

                drawFullInstNames = false;

                startMeasure = i;
                systemIndex++;

                if (systemIndex < song->systems.size())
                {
                    systemPosition.x = pagePosition.x + song->systems[systemIndex]->position.x;
                    systemPosition.y = pagePosition.y + song->systems[systemIndex]->position.y;
                }
            } // system loop (sort of)

            if (song->GetPageIndex(i) > page) // don't render the next page
            {
                break;
            }

            if (systemIndex >= systemPositions.size())
                systemPositions.push_back(systemPosition);
        }
    }
}

void MusicRenderer::RenderDebugToPage(const std::shared_ptr<Song>& song, int page, RenderData& pageRenderData, const Settings& settings, float pageX, float pageY)
{
    if (updateRenderData)
    {
        int measureCount = song->GetMeasureCount();

        int startingMeasureIndex = song->GetFirstMeasureOnPage(page);

        int startMeasure = startingMeasureIndex;
        int endMeasure = startingMeasureIndex-1;

        int systemIndex = song->GetSystemIndex(startingMeasureIndex);

        bool drawFullInstNames = true;

        Vec2<float> systemPosition = { pageX + song->displayConstants.leftMargin + song->systems[systemIndex]->layout.systemLeftMargin,
                                       pageY + song->displayConstants.topMargin + song->systems[systemIndex]->layout.topSystemDistance};

        systemPosition.x = pageX + song->systems[systemIndex]->position.x;
        systemPosition.y = pageY + song->systems[systemIndex]->position.y;

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

            if ((startNewSystem || i == measureCount) && i != 0 && systemIndex < song->systems.size()) // render system
            {
                startMeasure = endMeasure+1;
                endMeasure = i-1;

                systemPosition = RenderDebugSystem(pageRenderData, settings, song, startMeasure, endMeasure, systemIndex, systemPosition, { pageX, pageY }, drawFullInstNames);

                drawFullInstNames = false;

                startMeasure = i;
                systemIndex++;

                if (systemIndex < song->systems.size())
                {
                    systemPosition.x = pageX + song->systems[systemIndex]->position.x;
                    systemPosition.y = pageY + song->systems[systemIndex]->position.y;
                }
            } // system loop (sort of)

            if (song->GetPageIndex(i) > page) // don't render the next page
            {
                break;
            }

            if (systemIndex >= systemPositions.size())
                systemPositions.push_back(systemPosition);
        }
    }
}

Vec2<float> MusicRenderer::RenderSystem(RenderData& renderData, const Settings& settings, const std::shared_ptr<Song>& song, unsigned int startMeasure, unsigned int endMeasure, int systemIndex, Vec2<float> systemPosition, Vec2<float> pagePosition, bool drawFullInstNames)
{
    std::shared_ptr<Instrument> prevInstrument = nullptr;
    int instrumentIndex = 0;
    float instPositionY = systemPosition.y;
    for (const auto& instrument: song->instruments)
    {
        if (song->songData.instrumentInfos[instrumentIndex].visible)
        {
            instPositionY = systemPosition.y + instrument->systemPositionData[systemIndex].y;

            if (song->instruments.size() > 1)
            {
                float textPositionY = instPositionY + (instrument->GetMiddleHeight(song->displayConstants, systemIndex) / 2.0f);
                if (drawFullInstNames)
                {
                    renderData.AddText(Text(instrument->name.string, { systemPosition.x - 20.0f, textPositionY }, InstNameTextPaint));
                }
                else
                {
                    renderData.AddText(Text(instrument->nameAbbreviation.string, { systemPosition.x - 20.0f, textPositionY }, InstNameTextPaint));
                }
            }

            int staffIndex = 0;
            for (const auto& staff : instrument->staves)
            {
                float ls = song->displayConstants.lineSpacing;
                if (staff->type == Staff::StaffType::Tab) {
                    ls = song->displayConstants.tabLineSpacing;
                }

                float staffPositionY = staff->systemPositionData[systemIndex].y;

                for (const auto& direction : staff->durationDirections)
                {
                    if (direction->startMeasureIndex >= startMeasure && direction->endMeasureIndex <= endMeasure ||     // completely inside
                        direction->startMeasureIndex <= startMeasure && direction->endMeasureIndex >= endMeasure ||     // completely outside
                        direction->startMeasureIndex <= startMeasure && direction->endMeasureIndex >= startMeasure ||   // partly left
                        direction->startMeasureIndex <= endMeasure && direction->endMeasureIndex >= endMeasure)         // partly right
                    {
                        int startMeasureIndex = std::max(direction->startMeasureIndex, (int)startMeasure);
                        int endMeasureIndex = std::min(direction->endMeasureIndex, (int)endMeasure);

                        float startMeasurePositionX = song->GetMeasurePositionX(startMeasureIndex) + systemPosition.x;
                        float endMeasurePositionX = song->GetMeasurePositionX(endMeasureIndex) + systemPosition.x;

                        direction->Render(renderData, staffPositionY + instPositionY, startMeasurePositionX, endMeasurePositionX, startMeasureIndex, endMeasureIndex);
                    }
                }

                RenderLineOfMeasures(renderData, settings, startMeasure, endMeasure, song->systems[systemIndex], staff, systemPosition.x, staffPositionY + instPositionY, ls, instrumentIndex == 0 && staffIndex == 0, song->endingGroups);

                staffIndex++;
            } // staves loop

            if (instrument->instrumentBracket)
            {
                float height = instrument->GetMiddleHeight(10.0f, 13.33f, startMeasure, endMeasure);
                height = instrument->staves[instrument->staves.size() - 1]->systemPositionData[systemIndex].y;
                float lineSpacing = renderData.displayConstants.lineSpacing;
                if (instrument->staves[instrument->staves.size() - 1]->type == Staff::StaffType::Tab)
                    lineSpacing = renderData.displayConstants.tabLineSpacing;
                height += instrument->staves[instrument->staves.size() - 1]->GetMiddleHeight(lineSpacing);

                instrument->instrumentBracket->Render(renderData, { systemPosition.x, instPositionY }, height);
            }

            prevInstrument = instrument;
        } // instrument is visible

        instrumentIndex++;
    } // instruments loop

    if (song->instruments.size() > 1) // more than one instrument
    {
        // system connecting line
        renderData.AddLine(Line(systemPosition, { systemPosition.x, systemPosition.y + song->GetSystemHeight(systemIndex) }, BarLinePaint));
    }

    for (const auto& endingGroup : song->endingGroups)
    {
        for (const auto& ending : endingGroup->endings)
        {
            if (DoBoundsCollide(ending->startMeasureIndex, ending->endMeasureIndex, (int)startMeasure, (int)endMeasure))
            {
                int startMeasureIndex = std::max(ending->startMeasureIndex, (int)startMeasure);
                int endMeasureIndex = std::min(ending->endMeasureIndex, (int)endMeasure);

                float startMeasurePositionX = song->GetMeasurePositionX(startMeasureIndex) + systemPosition.x;
                float endMeasurePositionX = song->GetMeasurePositionX(endMeasureIndex) + systemPosition.x;

                float endMeasureWidth = song->GetMeasureWidth(endMeasureIndex);
                Vec2<float> measureStartPosition = { startMeasurePositionX, systemPosition.y };
                Vec2<float> measureEndPosition = { endMeasurePositionX + endMeasureWidth, systemPosition.y };

                if (ending)
                    ending->Render(renderData, measureStartPosition, measureEndPosition, startMeasureIndex, endMeasureIndex);
            }
        }
    }

    return systemPosition;
}

Vec2<float> MusicRenderer::RenderDebugSystem(RenderData& renderData, const Settings& settings, const std::shared_ptr<Song>& song, unsigned int startMeasure, unsigned int endMeasure, int systemIndex, Vec2<float> systemPosition, Vec2<float> pagePosition, bool drawFullInstNames)
{
    std::shared_ptr<Instrument> prevInstrument = nullptr;
    int instrumentIndex = 0;
    float instPositionY = systemPosition.y;
    for (const auto& instrument: song->instruments)
    {
        if (song->songData.instrumentInfos[instrumentIndex].visible)
        {
            instPositionY = systemPosition.y + instrument->systemPositionData[systemIndex].y;

            /*if (song->instruments.size() > 1)
            {
                float textPositionY = instPositionY + (instrument->GetMiddleHeight(song->displayConstants.lineSpacing, song->displayConstants.tabLineSpacing, 0, 1) / 2.0f);
                if (drawFullInstNames)
                {
                    renderData.AddText(Text(instrument->name.string, systemPosition.x - 20.0f, textPositionY, InstNameTextPaint));
                }
                else
                {
                    renderData.AddText(Text(instrument->nameAbbreviation.string, systemPosition.x - 20.0f, textPositionY, InstNameTextPaint));
                }
            }*/

            int staffIndex = 0;
            for (const auto& staff : instrument->staves)
            {
                float ls = song->displayConstants.lineSpacing;
                if (staff->type == Staff::StaffType::Tab) {
                    ls = song->displayConstants.tabLineSpacing;
                }

                // staff y position
                float staffYPosition = 0.0f;
                staffYPosition = staff->systemPositionData[systemIndex].y;

                for (const auto& direction : staff->durationDirections)
                {
                    if (direction->startMeasureIndex >= startMeasure && direction->endMeasureIndex <= endMeasure ||     // completely inside
                        direction->startMeasureIndex <= startMeasure && direction->endMeasureIndex >= endMeasure ||     // completely outside
                        direction->startMeasureIndex <= startMeasure && direction->endMeasureIndex >= startMeasure ||   // partly left
                        direction->startMeasureIndex <= endMeasure && direction->endMeasureIndex >= endMeasure)         // partly right
                    {
                        int startMeasureIndex = std::max(direction->startMeasureIndex, (int)startMeasure);
                        int endMeasureIndex = std::min(direction->endMeasureIndex, (int)endMeasure);

                        float startMeasurePositionX = song->GetMeasurePositionX(startMeasureIndex) + systemPosition.x;
                        float endMeasurePositionX = song->GetMeasurePositionX(endMeasureIndex) + systemPosition.x;

                        direction->RenderDebug(renderData, staffYPosition + instPositionY, startMeasurePositionX, endMeasurePositionX, startMeasureIndex, endMeasureIndex);
                    }
                }

                RenderDebugLineOfMeasures(renderData, settings, startMeasure, endMeasure, song->systems[systemIndex], staff, systemPosition.x, staffYPosition + instPositionY, ls, instrumentIndex == 0 && staffIndex == 0);

                staffIndex++;
            } // staves loop

            prevInstrument = instrument;
        } // instrument is visible

        instrumentIndex++;
    } // instruments loop

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

void MusicRenderer::RenderLineOfMeasures(RenderData& renderData, const Settings& settings, unsigned int startMeasure, unsigned int endMeasure, const std::shared_ptr<System>& system, const std::shared_ptr<Staff>& staff, float systemPositionX, float staffPositionY, float lineSpacing, bool isTopMeasureLine, const std::vector<std::shared_ptr<EndingGroup>>& endingGroups)
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
    for (int m = (int)startMeasure; m <= endMeasure; m++)
    {
        /*if (staff->csStaff)
        {
            if (m < staff->csStaff->measures.size())
            {
                CSMeasure csMeasure = staff->csStaff->measures[m];

                csMeasure.Render(renderData, settings, { measurePositionX, staffPositionY });
            }
        }*/

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
                multiMeasureRest = true;
                numberOfMeasuresInMultiMeasureRest = measure->multiMeasureRestSymbol->numberOfMeasures;
                measureThatStartedMultiMeasureRest = measureIndex;
            }

            bool isPartOfEnding = false;
            for (const auto& endingGroup : endingGroups)
            {
                for (const auto& ending : endingGroup->endings)
                {
                    if (ending->startMeasureIndex <= measureIndex && ending->endMeasureIndex >= measureIndex)
                        isPartOfEnding = true;
                }
            }

            measure->Render(renderData, settings, { measurePositionX, staffPositionY }, nextMeasurePositionX, system, staff->lines, lineSpacing, isTopMeasureLine, isLastMeasureInSystem, staff->tablatureDisplayType, isPartOfEnding);

            currentMeasureRenderedCount++;
        }

        measurePositionX = nextMeasurePositionX;
        measureNumber++;
        measureIndex++;
    } // measures loop
}

void MusicRenderer::RenderDebugLineOfMeasures(RenderData& renderData, const Settings& settings, unsigned int startMeasure, unsigned int endMeasure, const std::shared_ptr<System>& system, const std::shared_ptr<Staff>& staff, float systemPositionX, float staffPositionY, float lineSpacing, bool isTopMeasureLine)
{
    bool multiMeasureRest = false; // whether the measure is part of a multi measure rest
    unsigned int numberOfMeasuresInMultiMeasureRest = 0; // number of measures left in multi measure rest
    unsigned int measureThatStartedMultiMeasureRest = 0; // the index of the measure that started the multi measure rest

    int measureIndex = (int)startMeasure;
    int measureNumber = (int)startMeasure;
    float measurePositionX = systemPositionX;
    float nextMeasurePositionX = 0.0f;
    int measureRenderCount = 15; // the number of measures that need to be rendered
    int currentMeasureRenderedCount = 0; // the number of measures that have been rendered
    bool isLastMeasureInSystem = false;
    for (int m = (int)startMeasure; m <= endMeasure; m++)
    {
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
                multiMeasureRest = true;
                numberOfMeasuresInMultiMeasureRest = measure->multiMeasureRestSymbol->numberOfMeasures;
                measureThatStartedMultiMeasureRest = measureIndex;
            }

            measure->RenderDebug(renderData, settings, { measurePositionX, staffPositionY }, nextMeasurePositionX, system, staff->lines, lineSpacing, isTopMeasureLine, isLastMeasureInSystem, staff->tablatureDisplayType);

            currentMeasureRenderedCount++;
        }

        measurePositionX = nextMeasurePositionX;
        measureNumber++;
        measureIndex++;
    } // measures loop
}

void MusicRenderer::RenderCredits(RenderData& renderData, const std::shared_ptr<Song>& song, const MusicDisplayConstants& displayConstants, const std::vector<Credit>& credits, float pageX, float pageY)
{
    for (const Credit& credit : credits)
    {
        credit.Render(renderData, { pageX, pageY }, { song->displayConstants.pageWidth, song->displayConstants.pageHeight });
    }
}

int MusicRenderer::OnCalculateNumPages(const std::shared_ptr<Song>& song)
{
    int numPages = song->GetNumPages();

    if (numPages == 0)
        LOGE("NUM PAGES IS 0");

    LOGI("There are %d pages", numPages);

    return numPages;
}