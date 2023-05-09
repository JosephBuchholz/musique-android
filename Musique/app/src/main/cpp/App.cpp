/**
 * This file contains implementations of the non-rendering functions in App.h
 */

#include "App.h"
#include "Utils/Converters.h"
#include "MusicXMLParser/MusicXMLParser.h"
#include "Debuging/Debug.h"

App::App()
{
    musicRenderer = std::make_shared<MusicRenderer>();
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
    musicRenderer->layoutCalculated = false;
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
    song = std::make_shared<Song>();
    MusicXMLParser::ParseMusicXML(string, error, song);
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
        musicRenderer->updateRenderData = true;
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
                                        musicRenderer->updateRenderData = true;
                                    }
                                } else if (note->isPlaying) {
                                    note->OnStop();
                                    midi.ChangeInstrument(instrument->midiInstrument.program,
                                                          instrument->midiInstrument.channel);
                                    midi.StopNote(note, measure,
                                                  instrument->midiInstrument.channel);
                                    musicRenderer->updateRenderData = true;
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

                song->UpdateBoundingBoxes(musicRenderer->pagePositions, song->GetSystemPositions());
                song->ResolveCollisions();
                songUpdated = true;
            }

            musicRenderer->Render(song, settings);
        }

    } // if (isUpdating)
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

bool App::OnUpdatePrintLayout()
{
    LOGI("ON UPDATE PRINT LAYOUT!!");
    PrintRenderData printRenderData = PrintRenderData();
    bool layoutChanged = true;

    int totalPages = musicRenderer->OnCalculateNumPages(song);
    int pageIndex = 0;
    while (pageIndex < totalPages)
    {
        RenderData pageRenderData = RenderData();
        musicRenderer->updateRenderData = true;
        musicRenderer->RenderMusicToPage(song, pageIndex, pageRenderData, 0.0f, 0.0f);
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
    song->OnLayoutChanged(settings.musicLayout);
    //m_RenderData = RenderData(); // reduntant
    musicRenderer->updateRenderData = true;
    musicRenderer->layoutCalculated = false;

    LOGI("NEW layout: %d", (int)settings.musicLayout);
}

void App::OnInputEvent(const InputEvent& event)
{
    switch (event.type)
    {
        case InputEvent::InputEventType::Tap:
        {
            Measure* selectedMeasure = song->GetMeasureAtPoint(event.position);

            if (selectedMeasure != nullptr)
            {
                LOGD("A measure was selected! %d", selectedMeasure->index);

                playLineBeatPosition = selectedMeasure->beatPosition;
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