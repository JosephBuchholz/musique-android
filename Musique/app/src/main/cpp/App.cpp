#include "App.h"
#include "MusicData/DisplayConstants.h"
#include "Utils/Converters.h"
#include "MusicXMLParser.h"

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
    BarLinePaint.strokeCap = Paint::Cap::Square;

    TextPaint = Paint();
    TextPaint.textSize = 30.0f;

    TabTextPaint = Paint();
    TabTextPaint.textSize = 30.0f;
    TabTextPaint.textSize = 30.0f;
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
    DeleteSongData();
    LOGI("App Done Deconstructing");
}

void App::LoadSongFromString(const std::string& string)
{
    isUpdating = false;
    playing = false;
    playLineBeatPosition = 0.0f;
    currentMeasure = 0;

    LOGI("Loading song from string");
    if (songData != nullptr)
    {
        DeleteSongData();
    }

    std::string error = "";
    songData = ParseMusicXML(string, error);
    if (error != "")
    {
        LOGE("Error when parsing music XML: %s", error.c_str());
    }
    else
    {
        LOGI("There was no error when parsing music XML");
    }

    int numStaves = 0;
    int numMeasures = 0;
    int numNotes = 0;
    if (songData != nullptr) {
        for (Instrument *inst : songData->instruments) {
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

    if (error == "")
        LOGI("instruments: %d, staves: %d, measures: %d, notes: %d", songData->instruments.size(), numStaves, numMeasures, numNotes);

    if (songData != nullptr)
        songData->OnUpdate();

    // crash error is caused when isUpdating = true
    if (error == "")
        isUpdating = true;
}

void App::DeleteSongData()
{
    LOGI("Deleting Song Data");
    for (auto* instrument : songData->instruments) {
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
    songData->instruments.clear();
    delete songData;
    songData = nullptr;
}

void App::OnMidiStart()
{
    midi.ChangeInstrument((int)Midi::MidiInstrumentType::AcousticGrandPiano, 0);
}

void App::OnUpdate(double dt)
{
    double dts = dt / 1000.0;
    double dtm = dts / 60.0;
    aTime += dts;
    frames += 1;
    if (aTime >= 1.0f) {
        fps = frames;
        aTime = aTime - 1.0f;
        frames = 0;
    }
    if (icount >= 500000) {
        LOGI("Hello from OnUpdate!: %f, fps: %f", dt, fps);
        jcount += 1;
        icount = 0;
    }
    icount += 1;

    if (isUpdating) {
        if (playing) {
            playLineBeatPosition += currentTempo * dtm;
        }

        float playLinePosInMeasure = 0.0f;
        if (playing) {
            for (auto *instrument : songData->instruments) {
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
                                    }
                                } else if (note->isPlaying) {
                                    note->OnStop();
                                    midi.ChangeInstrument(instrument->midiInstrument.program,
                                                          instrument->midiInstrument.channel);
                                    midi.StopNote(note, measure,
                                                  instrument->midiInstrument.channel);
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

        if (startRendering) {
            RenderData renderData = RenderData();
            FrameData frameData = FrameData();

            //std::vector<float> measureWidths;

            //for (auto* instrument : songData->instruments) {
            //    for (auto* staff : instrument->staves) {
            //        int measureNumber = 0;
            //        for (const auto* measure : staff->measures) {
            //           int minWidth = measure->CalculateMinWidth();
//
//                        if ((int)measureWidths.size() - 1 >= measureNumber) { // if there is all ready a width at measureNumber
//                            if (measureWidths[measureNumber] < minWidth) { // if the width of this measure is bigger than the widths of the previous measures
//                               measureWidths[measureNumber] = minWidth;
 //                           }
 //                       } else { // else push back width
  //                          measureWidths.push_back(minWidth);
   //                     }
//
//                        measureNumber++;
//                    }
//                }
 //           }

            // --- MAIN RENDERING ---
            float instYPosition = 20.0f;
            for (auto *instrument : songData->instruments) {
                int staffIndex = 0;
                for (auto *staff : instrument->staves) {
                    float lineSpacing = 10.0f;
                    float tabLineSpacing = 13.33f;

                    if (staff->type == Staff::StaffType::Tab) {
                        lineSpacing = tabLineSpacing;
                    } else {
                    }

                    // staff lines
                    float staffYPosition = 0.0f;
                    if (staffIndex == 0) {
                        staffYPosition = 0.0f;
                    } else if (staffIndex == 1) {
                        staffYPosition = (lineSpacing * staff->lines) * 2.0f;
                    }
                    float songWidth = songData->GetSongWidth();
                    for (int i = 0; i < staff->lines; i++) {
                        renderData.AddLine(
                                Line(0.0f, (lineSpacing * i) + staffYPosition + instYPosition,
                                     songWidth, (lineSpacing * i) + staffYPosition + instYPosition,
                                     BarLinePaint));
                    }

                    //for (int i = 0; i < staff->lines; i++) {
                    //    renderData.AddLine(Line(0.0f, (lineSpacing * i) + staffYPosition + instYPosition, displayWidth, (lineSpacing * i) + staffYPosition + instYPosition, BarLinePaint));
                    //}

                    int measureNumber = 0;
                    float measurePosition = 0.0f;
                    for (auto *measure : staff->measures) {

                        // measure lines
                        float x = measurePosition;
                        //if (measureNumber != 0) {
                        //    x = measureWidths[measureNumber];
                        //}
                        renderData.AddLine(
                                Line(x, 0.0f + staffYPosition + instYPosition, x,
                                     (lineSpacing * (staff->lines - 1)) + staffYPosition +
                                     instYPosition, BarLinePaint));
                        x += songData->GetMeasureWidth(measureNumber);
                        renderData.AddLine(
                                Line(x, 0.0f + staffYPosition + instYPosition, x,
                                     (lineSpacing * (staff->lines - 1)) + staffYPosition +
                                     instYPosition, BarLinePaint));

                        if (measureNumber == currentMeasure) {
                            // calculating play line position in measure
                            playLinePosInMeasure = measure->GetPlayLinePositionInMeasure(
                                    playLineBeatPosition - currentMeasureBeatPosition,
                                    songData->GetMeasureWidth(measureNumber));
                            playLinePosition = measurePosition + playLinePosInMeasure;
                        }

                        // time signature
                        if (measure->showTimeSignature and measure->timeSignature.print) {
                            float positionX = measure->GetTimeSignaturePositionInMeasure(
                                    songData->GetMeasureWidth(measureNumber)) + measurePosition;
                            float positionY = 20.0f + staffYPosition + instYPosition;
                            renderData.AddBitmap(
                                    RenderBitmap(
                                            GetTimeSignatureAssetID(measure->timeSignature.notes),
                                            positionX, positionY, 1.0f, 1.0f,
                                            Paint(0xff000000)));
                            positionY = 40.0f + staffYPosition + instYPosition;
                            renderData.AddBitmap(
                                    RenderBitmap(GetTimeSignatureAssetID(
                                            measure->timeSignature.noteType), positionX, positionY,
                                                 1.0f, 1.0f,
                                                 Paint(0xff000000)));
                        }

                        // clef
                        if (measure->showClef) {
                            float positionX = measure->GetClefPositionInMeasure(
                                    songData->GetMeasureWidth(measureNumber)) + measurePosition;
                            float positionY =
                                    (lineSpacing * float(staff->lines - 1)) + staffYPosition +
                                    instYPosition; // the bottom line + instYPosition
                            renderData.AddBitmap(
                                    RenderBitmap(GetClefAssetID(measure->clef), positionX,
                                                 positionY, 1.0f, 1.0f,
                                                 Paint(0xff000000)));
                        }

                        // key signature
                        if (measure->showKeySignature && measure->keySignature.fifths != 0) {
                            float positionX = measure->GetKeySignaturePositionInMeasure(
                                    songData->GetMeasureWidth(measureNumber)) + measurePosition;
                            float positionY =
                                    (lineSpacing * float(staff->lines - 1)) + staffYPosition +
                                    instYPosition; // the bottom line + instYPosition
                            renderData.AddBitmap(
                                    RenderBitmap(
                                            GetKeySignatureAssetID(measure->keySignature.fifths),
                                            positionX, positionY, 1.0f, 1.0f,
                                            Paint(0xff000000)));
                        }

                        int noteIndex = 0;
                        float beamStartX = 0.0f;
                        float beamStartY = 0.0f;
                        for (Note *note : measure->notes) {
                            if (note->type == Note::NoteType::Tab /* && noteIndex != 0 && noteIndex != measure->notes.size() - 1 && noteIndex != measure->notes.size() - 2 && noteIndex != measure->notes.size() - 3 && noteIndex != measure->notes.size() - 4 && noteIndex != measure->notes.size() - 5*/) // tab note
                            {
                                //LOGI("tab note beat position: %f", note->beatPositionInSong);
                                float positionX = songData->GetPositionXInSong(note->beatPositionInSong, measureNumber); // this line of code crashes the program
                                //LOGI("Fret as string: %s", ToString(tabNote->fret).c_str());
                                //float position = note->beatPositionInSong;
                                //float position = (float)measureNumber;
                                if (note->isPlaying) {
                                    renderData.AddText(
                                            Text(ToString(note->fret), positionX,
                                                 (lineSpacing * note->string) + staffYPosition +
                                                 instYPosition,
                                                 Paint(0xff1188ee, TabTextPaint)));
                                } else {
                                    renderData.AddText(
                                            Text(ToString(note->fret), positionX,
                                                 (lineSpacing * note->string) + staffYPosition +
                                                 instYPosition,
                                                 Paint(0xff000000, TabTextPaint)));
                                }
                            } else/* if (noteIndex != 0 && noteIndex != measure->notes.size() - 1 && noteIndex != measure->notes.size() - 2 && noteIndex != measure->notes.size() - 3 && noteIndex != measure->notes.size() - 4 && noteIndex != measure->notes.size() - 5)*/ // standard note
                            {
                                if (!note->isRest) {
                                    // rendering note head
                                    //float positionX = measure->GetNotePositionInMeasure(
                                    //        songData->GetMeasureWidth(measureNumber), noteIndex) + measurePosition;
                                    float positionX = songData->GetPositionXInSong(note->beatPositionInSong, measureNumber);
                                    float positionY =
                                            (lineSpacing * measure->GetNoteYPosition(noteIndex)) +
                                            staffYPosition +
                                            instYPosition;
                                    if (note->isPlaying) {
                                        renderData.AddBitmap(
                                                RenderBitmap(GetNoteHeadAssetID(note->durationType),
                                                             positionX, positionY));
                                        //renderData.AddText(
                                        //        Text(ToString(0), positionX, positionY,
                                        //             Paint(0xff1188ee)));
                                    } else {
                                        renderData.AddBitmap(
                                                RenderBitmap(GetNoteHeadAssetID(note->durationType),
                                                             positionX, positionY, 1.0f, 1.0f,
                                                             Paint(0xff0044dd)));
                                        //renderData.AddText(
                                        //        Text(ToString(0), positionX, positionY,
                                        //             Paint(0xff000000)));
                                    }

                                    // rendering note stem
                                    float noteWidth = 11.3f;
                                    float stemEndY = 0.0f;
                                    if (note->noteStem.stemType == NoteStem::StemType::Up) {
                                        float px = positionX + noteWidth -
                                                   NoteStemPaint.strokeWidth / 2.0f;
                                        renderData.AddLine(
                                                Line(px, positionY, px, positionY - 30.0f,
                                                     NoteStemPaint));
                                        stemEndY = positionY - 30.0f;
                                    } else if (note->noteStem.stemType ==
                                               NoteStem::StemType::Down) {
                                        float px = positionX + NoteStemPaint.strokeWidth / 2.0f;
                                        renderData.AddLine(
                                                Line(px, positionY, px, positionY + 30.0f,
                                                     NoteStemPaint));
                                        stemEndY = positionY + 30.0f;
                                    } else if (note->noteStem.stemType ==
                                               NoteStem::StemType::Double) {
                                        float px = positionX + NoteStemPaint.strokeWidth / 2.0f;
                                        renderData.AddLine(
                                                Line(px, positionY, px, positionY + 30.0f,
                                                     NoteStemPaint));
                                        px = positionX + noteWidth -
                                             NoteStemPaint.strokeWidth / 2.0f;
                                        renderData.AddLine(
                                                Line(px, positionY, px, positionY - 30.0f,
                                                     NoteStemPaint));
                                    }

                                    // rendering note beam
                                    if (note->beam.beamType == Beam::BeamType::Begin) {
                                        if (note->noteStem.stemType == NoteStem::StemType::Up) {
                                            beamStartX = positionX + noteWidth;
                                            beamStartY = stemEndY;
                                        } else if (note->noteStem.stemType ==
                                                   NoteStem::StemType::Down) {
                                            beamStartX = positionX;
                                            beamStartY = stemEndY;
                                        }
                                    } else if (note->beam.beamType == Beam::BeamType::Continue) {
                                        // do something
                                    } else if (note->beam.beamType == Beam::BeamType::End) {
                                        float beamEndX = 0.0f;
                                        float beamEndY = stemEndY;
                                        if (note->noteStem.stemType == NoteStem::StemType::Up) {
                                            beamEndX = positionX + noteWidth;
                                        } else if (note->noteStem.stemType ==
                                                   NoteStem::StemType::Down) {
                                            beamEndX = positionX;
                                        }

                                        renderData.AddLine(
                                                Line(beamStartX, beamStartY, beamEndX, beamEndY,
                                                     NoteBeamPaint));
                                    }

                                    if (note->accidental.accidentalType !=
                                        Accidental::AccidentalType::None) {
                                        float accX = positionX - 10.0f;
                                        float accY = positionY;

                                        renderData.AddBitmap(RenderBitmap(
                                                GetAccidentalAssetID(
                                                        note->accidental.accidentalType),
                                                accX, accY, 1.0f, 1.0f, Paint(0xff000000)));
                                    }
                                } else // is a rest
                                {
                                    // rendering rest
                                    float positionX = songData->GetPositionXInSong(
                                            note->beatPositionInSong, measureNumber);
                                    float positionY =
                                            ((lineSpacing * (staff->lines - 1)) / 2.0f) +
                                            staffYPosition +
                                            instYPosition;

                                    if (note->durationType == Note::NoteDurationType::Quarter) {
                                        renderData.AddBitmap(
                                                RenderBitmap(AssetID::QuarterRest,
                                                             positionX, positionY));
                                    } else if (note->isFullMeasureRest) {
                                        positionY = lineSpacing + staffYPosition + instYPosition;
                                        renderData.AddBitmap(
                                                RenderBitmap(AssetID::WholeRest,
                                                             positionX, positionY));
                                    } else if (note->durationType == Note::NoteDurationType::Half) {
                                        positionY = (lineSpacing * 2.0f) + staffYPosition +
                                                    instYPosition;
                                        renderData.AddBitmap(
                                                RenderBitmap(AssetID::HalfRest,
                                                             positionX, positionY));
                                    }
                                }
                            }

                            noteIndex++;
                        }

                        measurePosition += songData->GetMeasureWidth(measureNumber);
                        measureNumber++;
                    }
                    staffIndex++;
                }

                instYPosition += 70.0f;
            }

            playLinePosition = songData->GetPositionXInSong(playLineBeatPosition, currentMeasure);

            renderData.AddLine(
                    Line(0.0f, 20.0f, 0.0f, instYPosition, BarLinePaint));

            if (playLineHeight == 0.0f) {
                playLineHeight = instYPosition + 80.0f;
            }

            frameData.playLinePosition = playLinePosition;
            frameData.playLinePositionY = playLineY;
            frameData.playLineHeight = playLineHeight;

            // play line
            //renderData.AddLine(Line(playLinePosition, playLineY, playLinePosition, playLineHeight, Paint(0xff0044dd, BarLinePaint)));

            UpdateRender(renderData);
            UpdateFrameData(frameData);
        }
    }
}

void App::SetViewModelData(ViewModelData viewModelData)
{
    playing = viewModelData.playing;
    playLineBeatPosition = viewModelData.playLineBeatPosition;
    currentMeasure = viewModelData.currentMeasure;
}
