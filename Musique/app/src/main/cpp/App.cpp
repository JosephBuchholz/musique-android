/**
 * This file contains implementations of the non-rendering functions in App.h
 */

#include "App.h"
#include "Utils/Converters.h"
#include "FileParsers/MusicXMLParser/MusicXMLParser.h"
#include "Debuging/Debug.h"
#include "Exceptions/Exceptions.h"
#include "FileParsers/ChordSheetParser/ChordSheetParser.h"

#include <chrono>
using namespace std::chrono;

App::App()
{
    musicRenderer = std::make_shared<MusicRenderer>();
    musicPlayer = std::make_shared<MusicPlayer>();
    musicPlayer->SetOnTempoChangedCallback(OnTempoChangedCallback);
}

App::~App()
{
    isUpdating = false;
    LOGI("App Deconstructer");
    ViewModelData viewModelData = ViewModelData();
    //viewModelData.playing = musicPlayer->IsPlaying();
    viewModelData.playLineBeatPosition = musicPlayer->GetPlayLineBeatPosition();
    viewModelData.currentMeasure = musicPlayer->GetCurrentMeasure();
    UpdateViewModelData(viewModelData);
    LOGI("App Done Deconstructing");
}

void App::LoadSongFromString(const std::string& extension, const std::string& string)
{
    isUpdating = false;
    musicRenderer->layoutCalculated = false;
    startRendering = false;
    songUpdated = false;
    musicPlayer->Reset();

    LOGI_TAG("App", "Loading song from string");

    if (extension == "musicxml")
    {
        song = std::make_shared<Song>();
        MusicXMLParser::ParseMusicXML(string, song);

        unsigned int numStaves = 0;
        unsigned int numMeasures = 0;
        unsigned int numNotes = 0;
        if (song != nullptr)
        {
            song->settings = settings;

            for (const auto& inst: song->instruments)
            {
                numStaves += inst->staves.size();
                for (const auto& staff: inst->staves)
                {
                    numMeasures += staff->measures.size();
                    for (const auto& measure: staff->measures)
                    {
                        numNotes += measure->notes.size();
                    }
                }

                //midi.SetVolume(inst->midiInstrument.volume);
            }
        }

        if (song != nullptr)
        {
            //song->OnUpdate();
            UpdateSongData(song->songData);
        }

        // crash error is caused when isUpdating = true
        isUpdating = true;
        musicRenderer->updateRenderData = true;
        //startRendering = true;

        songLoaded = true;
        allowRendering = true;
        allowFrameDataRendering = false;
        updateMusicPlayer = true;
        doCollisions = true;
    }
    else if (extension == "txt")
    {
        LOGI("Text file format!!!!");

        song = std::make_shared<Song>();
        ChordSheetParser::ParseChordSheet(string, song);

        if (song)
        {
            song->settings = settings;
            UpdateSongData(song->songData);
        }

        songUpdated = false;
        isUpdating = true;
        musicRenderer->updateRenderData = true;
        updateMusicPlayer = false;
        allowRendering = true;
        allowFrameDataRendering = false;
        songLoaded = true;
        doCollisions = false;
    }
}

void App::OnMidiStart()
{
    musicPlayer->OnStart();
}

void App::OnUpdate(double dt)
{
    double dts = dt / 1000.0; // delta time in seconds
    double dtm = dts / 60.0; // delta time in minutes

    if (isUpdating)
    {
        if (updateMusicPlayer)
            musicPlayer->OnUpdate(dt, song);

        // --- RENDERING ---
        if (startRendering && allowRendering)
        {
            if (song != nullptr && !songUpdated)
            {
                auto startTotal = high_resolution_clock::now();
                {
                    auto start = high_resolution_clock::now();
                    song->OnUpdate();
                    auto stop = high_resolution_clock::now();
                    auto duration = duration_cast<milliseconds>(stop - start);
                    LOGD("Time taken by song->OnUpdate function: %lld milliseconds | %f seconds", duration.count(), duration.count() / 1000.0f);
                }

                int totalPages = musicRenderer->OnCalculateNumPages(song);
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

                    musicRenderer->pagePositions.emplace_back(pageX, pageY);
                    pageIndex++;
                }

                if (doCollisions)
                {
                    {
                        auto start = high_resolution_clock::now();
                        try
                        {
                            song->UpdateBoundingBoxes(musicRenderer->pagePositions, song->GetSystemPositions());
                        }
                        catch (const IsEmptyException &e)
                        {
                            LOGE("%s", e.what());
                        }
                        auto stop = high_resolution_clock::now();
                        auto duration = duration_cast<milliseconds>(stop - start);
                        LOGD("Time taken by UpdateBoundingBoxes function: %lld milliseconds | %f seconds", duration.count(), duration.count() / 1000.0f);
                    }

                    auto totalStart = high_resolution_clock::now();
                    for (int i = 0; i < 5; i++)
                    {
                        LOGD("Start resolving collisions (number %d)", i);
                        auto start = high_resolution_clock::now();
                        song->ResolveCollisions();
                        auto stop = high_resolution_clock::now();
                        auto duration = duration_cast<milliseconds>(stop - start);
                        LOGD("Time taken by collision function: %lld milliseconds | %f seconds", duration.count(), duration.count() / 1000.0f);
                    }
                    auto totalStop = high_resolution_clock::now();
                    auto totalDuration = duration_cast<milliseconds>(totalStop - totalStart);
                    LOGD("Total time taken by collision resolver: %lld milliseconds | %f seconds", totalDuration.count(), totalDuration.count() / 1000.0f);

                    {
                        auto start = high_resolution_clock::now();
                        song->CalculateSystemPositionsAndPageBreaks();
                        auto stop = high_resolution_clock::now();
                        auto duration = duration_cast<milliseconds>(stop - start);
                        LOGD("Time taken by CalculateSystemPositionsAndPageBreaks function: %lld milliseconds | %f seconds", duration.count(), duration.count() / 1000.0f);
                    }
                }

                songUpdated = true;

                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - startTotal);
                LOGD("|--------- Time taken by full song initialization: %lld milliseconds | %f seconds", duration.count(), duration.count() / 1000.0f);
            }

            /*{
                auto start = high_resolution_clock::now();

                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - start);
                LOGE("Time taken by Render function: %lld milliseconds | %f seconds", duration.count(), duration.count() / 1000.0f);
            }*/


            musicRenderer->Render(song, settings);

            if (allowFrameDataRendering)
            {
                FrameData frameData = FrameData();

                int currentSystemIndex = song->GetSystemIndex(musicPlayer->GetCurrentMeasure());
                int currentPageIndex = song->GetPageIndex(musicPlayer->GetCurrentMeasure());
                playLinePosition = musicRenderer->systemPositions[currentSystemIndex].x + song->GetMeasurePositionX(musicPlayer->GetCurrentMeasure());
                playLineY = musicRenderer->systemPositions[currentSystemIndex].y;

                playLineHeight = song->GetSystemHeight(currentSystemIndex);

                frameData.playLinePosition = playLinePosition;
                frameData.playLinePositionY = playLineY;
                frameData.playLineHeight = playLineHeight;
                frameData.playProgress = musicPlayer->GetPlayLineBeatPosition() / song->totalBeatWidth;
                frameData.isPlaying = musicPlayer->IsPlaying();

                UpdateFrameData(frameData);
            }
        }

    } // if (isUpdating)
}


void App::SetViewModelData(ViewModelData viewModelData)
{
    /*if (viewModelData.playing)
        musicPlayer->OnPlay();
    else
        musicPlayer->OnStop();*/
    musicPlayer->SetPlayLineBeatPosition(viewModelData.playLineBeatPosition);
    musicPlayer->SetCurrentMeasure(viewModelData.currentMeasure);
}

void App::OnPlayProgressChanged(float progress)
{
    musicPlayer->SetPlayLineBeatPosition(song->totalBeatWidth * progress);\
}

bool App::OnUpdatePrintLayout()
{
    LOGI("ON UPDATE PRINT LAYOUT!!");
    PrintRenderData printRenderData = PrintRenderData();
    bool layoutChanged = true;

    musicRenderer->systemPositions.clear();
    musicRenderer->pagePositions.clear();

    int totalPages = musicRenderer->OnCalculateNumPages(song);
    int pageIndex = 0;
    while (pageIndex < totalPages)
    {
        RenderData pageRenderData = RenderData();
        musicRenderer->updateRenderData = true;
        musicRenderer->RenderMusicToPage(song, pageIndex, pageRenderData, settings, { 0.0f, 0.0f });
        printRenderData.pages.push_back(pageRenderData);
        pageIndex++;
    }

    UpdatePrintRenderData(printRenderData);
    return layoutChanged;
}

void App::UpdateInstrumentInfo(const InstrumentInfo& info, unsigned int index)
{
    if (song && song->songData.instrumentInfos.size() > index) {
        if (song->songData.instrumentInfos[index].visible != info.visible) // visible field changed, so update render
            musicRenderer->updateRenderData = true;
        song->songData.instrumentInfos[index] = info;
    }
}

void App::OnLayoutChanged()
{
    LOGD("OnLayoutChanged!");
    if (song != nullptr)
    {
        song->settings = settings;
        if (songLoaded)
        {
            song->OnLayoutChanged(settings.musicLayout);
            //m_RenderData = RenderData(); // reduntant
            musicRenderer->updateRenderData = true;
            musicRenderer->layoutCalculated = false;

            LOGI("NEW layout: %d", (int) settings.musicLayout);
        }
    }
}

void App::OnInputEvent(const InputEvent& event)
{
    switch (event.type)
    {
        case InputEvent::InputEventType::Tap:
        {
            std::shared_ptr<Measure> selectedMeasure = song->GetMeasureAtPoint(event.position, musicRenderer->systemPositions);

            if (selectedMeasure != nullptr)
            {
                LOGD("A measure was selected! %d", selectedMeasure->index);

                musicPlayer->TravelToVisualBeatPosition(selectedMeasure->beatPosition, song);
            }

            LOGD("Received a tap event at: %s", event.position.GetPrintableString().c_str());
            //m_RenderData.AddDebugDot(event.position.x, event.position.y);
            //UpdateRender(m_RenderData);
            break;
        }
        default:
        {
            LOGW("RECEIVED UNRECOGNIZED EVENT");
            break;
        }
    }
}

int App::OnCalculateNumPages()
{
    return musicRenderer->OnCalculateNumPages(song);
}

void App::OnTranspose(const TranspositionRequest& transpositionRequest)
{
    LOGI("Got transposition request: %d", transpositionRequest.key.root);
    TranspositionRequest tr = transpositionRequest;
    tr.originalKey.root = MusicalKey::GetKeyFromFifths(song->instruments[0]->staves[0]->measures[0]->keySignature.fifths);
    if (song == nullptr)
        return;
    song->Transpose(tr);
    musicRenderer->updateRenderData = true;
    musicRenderer->layoutCalculated = false;
}
