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

    TabSlurPaint = Paint();
    TabSlurPaint.strokeWidth = 1.25f;

    TextPaint = Paint();
    TextPaint.textSize = 30.0f;

    TabTextPaint = Paint();
    TabTextPaint.textSize = 16.0f;

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
    double dts = dt / 1000.0; // delta time in seconds
    double dtm = dts / 60.0; // delta time in minutes
    aTime += dts;
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
                for (auto *instrument : songData->instruments) {

                    if (prevInstrument != nullptr)
                        instYPosition += prevInstrument->GetMiddleHeight(10.0f, 13.33f, 0, instrument->GetMeasureCount()) +
                                prevInstrument->GetBelowHeight(10.0f, 13.33f, 0, instrument->GetMeasureCount());
                    instYPosition += instrument->GetAboveHeight(10.0f, 13.33f, 0, instrument->GetMeasureCount());

                    int staffIndex = 0;
                    for (auto *staff : instrument->staves) {
                        float lineSpacing = 10.0f;
                        float tabLineSpacing = 13.33f;

                        if (staff->type == Staff::StaffType::Tab) {
                            lineSpacing = tabLineSpacing;
                        } else {
                        }

                        // staff y position
                        float staffYPosition = 0.0f;
                        if (staffIndex == 0) {
                            staffYPosition = 0.0f;
                        } else if (staffIndex == 1) {
                            staffYPosition = instrument->staves[staffIndex-1]->GetMiddleHeight(lineSpacing, 0, staff->measures.size()) +
                                    instrument->staves[staffIndex-1]->GetBelowHeight(lineSpacing, 0, staff->measures.size()) +
                                    staff->GetAboveHeight(lineSpacing, 0, staff->measures.size());
                        }

                        int measureNumber = 0;
                        float measurePosition = 0.0f;
                        int measureRenderCount = 15; // the number of measures that need to be rendered
                        int currentMeasureRenderedCount = 0; // the number of measures that have been rendered
                        float curveStartX = 0.0f;
                        float curveStartY = 0.0f;
                        for (auto *measure : staff->measures) {

                            if (!(currentMeasureRenderedCount >= measureRenderCount) &&
                                measureNumber >= currentMeasure - (measureRenderCount /
                                                                   2)) // render measure in a radius of measureRenderCount/2
                            {
                                float measureWidth = songData->GetMeasureWidth(measureNumber);

                                // staff lines
                                for (int i = 0; i < staff->lines; i++) {
                                    float endX = measurePosition + measureWidth;
                                    renderData.AddLine(
                                            Line(measurePosition,
                                                 (lineSpacing * i) + staffYPosition + instYPosition,
                                                 endX,
                                                 (lineSpacing * i) + staffYPosition + instYPosition,
                                                 BarLinePaint));
                                }


                                // measure lines (bar lines)
                                float x = measurePosition;
                                //if (measureNumber != 0) {
                                //    x = measureWidths[measureNumber];
                                //}
                                renderData.AddLine(
                                        Line(x, 0.0f + staffYPosition + instYPosition, x,
                                             (lineSpacing * float(staff->lines - 1)) + staffYPosition +
                                             instYPosition, BarLinePaint));
                                x += songData->GetMeasureWidth(measureNumber);
                                renderData.AddLine(
                                        Line(x, 0.0f + staffYPosition + instYPosition, x,
                                             (lineSpacing * float(staff->lines - 1)) + staffYPosition +
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
                                            songData->GetMeasureWidth(measureNumber)) +
                                                      measurePosition;
                                    float positionY = 20.0f + staffYPosition + instYPosition;
                                    renderData.AddBitmap(
                                            RenderBitmap(
                                                    GetTimeSignatureAssetID(
                                                            measure->timeSignature.notes),
                                                    positionX, positionY, 1.0f, 1.0f,
                                                    Paint(0xff000000)));
                                    positionY = 40.0f + staffYPosition + instYPosition;
                                    renderData.AddBitmap(
                                            RenderBitmap(GetTimeSignatureAssetID(
                                                    measure->timeSignature.noteType), positionX,
                                                         positionY,
                                                         1.0f, 1.0f,
                                                         Paint(0xff000000)));
                                }

                                // clef
                                if (measure->showClef) {
                                    float positionX = measure->GetClefPositionInMeasure(
                                            songData->GetMeasureWidth(measureNumber)) +
                                                      measurePosition;
                                    float positionY =
                                            (lineSpacing * float(staff->lines - 1)) +
                                            staffYPosition +
                                            instYPosition; // the bottom line + instYPosition
                                    renderData.AddBitmap(
                                            RenderBitmap(GetClefAssetID(measure->clef), positionX,
                                                         positionY, 1.0f, 1.0f,
                                                         Paint(0xff000000)));
                                }

                                // key signature
                                if (measure->showKeySignature &&
                                    measure->keySignature.fifths != 0) {
                                    float positionX = measure->GetKeySignaturePositionInMeasure(
                                            songData->GetMeasureWidth(measureNumber)) +
                                                      measurePosition;
                                    float positionY =
                                            (lineSpacing * float(staff->lines - 1)) +
                                            staffYPosition +
                                            instYPosition; // the bottom line + instYPosition
                                    renderData.AddBitmap(
                                            RenderBitmap(
                                                    GetKeySignatureAssetID(
                                                            measure->keySignature.fifths),
                                                    positionX, positionY, 1.0f, 1.0f,
                                                    Paint(0xff000000)));
                                }

                                // render directions
                                for (const Direction& direction : measure->directions)
                                {
                                    float positionY = staffYPosition + instYPosition - 20.0f;
                                    RenderDirection(renderData, direction, positionY, measure);
                                }

                                int noteIndex = 0;
                                float beamStartX = 0.0f;
                                float beamStartY = 0.0f;
                                for (Note *note : measure->notes) {

                                    // calculate color of the note
                                    int color = normalColor;
                                    if (note->isPlaying) {
                                        color = playedColor;
                                    }

                                    if (note->isRest) { // is a rest
                                        // rendering rest
                                        float positionX = songData->GetPositionXInSong(
                                                note->beatPositionInSong, measureNumber);
                                        float positionY =
                                                ((lineSpacing * float(staff->lines - 1)) / 2.0f) +
                                                staffYPosition +
                                                instYPosition;

                                        if (note->isFullMeasureRest) {
                                            positionY =
                                                    lineSpacing + staffYPosition +
                                                    instYPosition;
                                            renderData.AddBitmap(
                                                    RenderBitmap(AssetID::WholeRest,
                                                                 positionX, positionY, 1.0f, 1.0f, Paint(color)));
                                        } else if (note->durationType == Note::NoteDurationType::Whole) {
                                            positionY =
                                                    lineSpacing + staffYPosition +
                                                    instYPosition;
                                            renderData.AddBitmap(
                                                    RenderBitmap(AssetID::WholeRest,
                                                                 positionX, positionY, 1.0f, 1.0f, Paint(color)));
                                        } else if (note->durationType ==
                                                   Note::NoteDurationType::Half) {
                                            positionY = (lineSpacing * 2.0f) + staffYPosition +
                                                        instYPosition;
                                            renderData.AddBitmap(
                                                    RenderBitmap(AssetID::HalfRest,
                                                                 positionX, positionY, 1.0f, 1.0f, Paint(color)));
                                        } else
                                        {
                                            AssetID id;
                                            switch (note->durationType)
                                            {
                                                case Note::NoteDurationType::Quarter: id = AssetID::QuarterRest; break;
                                                case Note::NoteDurationType::Eighth: id = AssetID::EightRest; break;
                                                case Note::NoteDurationType::Sixteenth: id = AssetID::SixteenthRest; break;
                                                case Note::NoteDurationType::ThirtySecond: id = AssetID::ThirtySecondRest; break;
                                                default: id = AssetID::None; break;
                                            }
                                            renderData.AddBitmap(RenderBitmap(id, positionX, positionY, 1.0f, 1.0f, Paint(color)));
                                        }
                                    }
                                    else if (note->type ==
                                        Note::NoteType::Tab) // is a tab note
                                    {
                                        float positionX = songData->GetPositionXInSong(
                                                note->beatPositionInSong,
                                                measureNumber); // this line of code crashes the program
                                        float positionY = (lineSpacing * float(note->string-1)) + staffYPosition + instYPosition;
                                        renderData.AddText(
                                                Text(ToString(note->fret), positionX, positionY, Paint(color, TabTextPaint)));

                                        for (TABSlur slur : note->tabSlurs)
                                        {
                                            if (slur.type == StartStopType::Stop)
                                            {
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

                                            if (slur.type == StartStopType::Start)
                                            {
                                                curveStartX = positionX;
                                                curveStartY = positionY - 6.0f;
                                            }
                                        }
                                    } else // is a standard note
                                    {
                                        // rendering note head
                                        //float positionX = measure->GetNotePositionInMeasure(
                                        //        songData->GetMeasureWidth(measureNumber), noteIndex) + measurePosition;
                                        float positionX = songData->GetPositionXInSong(
                                                note->beatPositionInSong, measureNumber);
                                        float positionY =
                                                (lineSpacing *
                                                 measure->GetNoteYPosition(noteIndex)) +
                                                staffYPosition +
                                                instYPosition;

                                        renderData.AddBitmap(
                                                RenderBitmap(
                                                        GetNoteHeadAssetID(
                                                                note->durationType),
                                                        positionX, positionY, 1.0f, 1.0f, Paint(color)));

                                        // ledger lines
                                        if (measure->GetNoteYPosition(noteIndex) >= (float)staff->lines) // ledger lines below staff
                                        {
                                            int ledgerLineCount = (int)measure->GetNoteYPosition(noteIndex) - staff->lines + 1;
                                            float y = staffYPosition + instYPosition + ((float)staff->lines * lineSpacing);
                                            for (int i = 0; i < ledgerLineCount; i++) {
                                                renderData.AddLine(
                                                        Line(positionX - 4.0f, y, positionX + 14.0f, y,
                                                             BarLinePaint));
                                                y += 1.0f * lineSpacing;
                                            }
                                        }
                                        if (measure->GetNoteYPosition(noteIndex) < 0.0f) // ledger lines above staff
                                        {
                                            int ledgerLineCount = abs((int)measure->GetNoteYPosition(noteIndex));
                                            float y = staffYPosition + instYPosition - (1.0f * lineSpacing);
                                            for (int i = 0; i < ledgerLineCount; i++) {
                                                renderData.AddLine(
                                                        Line(positionX - 4.0f, y, positionX + 14.0f, y,
                                                             BarLinePaint));
                                                y -= 1.0f * lineSpacing;
                                            }
                                        }

                                        for (Slur slur : note->slurs)
                                        {
                                            if (slur.type == StartStopType::Stop)
                                            {
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

                                            if (slur.type == StartStopType::Start)
                                            {
                                                curveStartX = positionX;
                                                curveStartY = positionY - 6.0f;
                                            }
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
                                            float px = positionX +
                                                       NoteStemPaint.strokeWidth / 2.0f;
                                            renderData.AddLine(
                                                    Line(px, positionY, px, positionY + 30.0f,
                                                         NoteStemPaint));
                                            stemEndY = positionY + 30.0f;
                                        } else if (note->noteStem.stemType ==
                                                   NoteStem::StemType::Double) {
                                            float px = positionX +
                                                       NoteStemPaint.strokeWidth / 2.0f;
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
                                            if (note->noteStem.stemType ==
                                                NoteStem::StemType::Up) {
                                                beamStartX = positionX + noteWidth;
                                                beamStartY = stemEndY;
                                            } else if (note->noteStem.stemType ==
                                                       NoteStem::StemType::Down) {
                                                beamStartX = positionX;
                                                beamStartY = stemEndY;
                                            }
                                        } else if (note->beam.beamType ==
                                                   Beam::BeamType::Continue) {
                                            // do something
                                        } else if (note->beam.beamType == Beam::BeamType::End) {
                                            float beamEndX = 0.0f;
                                            float beamEndY = stemEndY;
                                            if (note->noteStem.stemType ==
                                                NoteStem::StemType::Up) {
                                                beamEndX = positionX + noteWidth;
                                            } else if (note->noteStem.stemType ==
                                                       NoteStem::StemType::Down) {
                                                beamEndX = positionX;
                                            }

                                            renderData.AddLine(
                                                    Line(beamStartX, beamStartY, beamEndX,
                                                         beamEndY,
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
                                    }

                                    for (const auto& lyric : note->lyrics)
                                    {
                                        float positionY = (lineSpacing * (float)staff->lines) +
                                                          staffYPosition +
                                                          instYPosition + 20.0f;
                                        RenderLyric(renderData, lyric, positionY, measure, note);
                                    } // lyrics loop

                                    noteIndex++;
                                } // notes loop

                                currentMeasureRenderedCount++;
                            }
                            measurePosition += songData->GetMeasureWidth(measureNumber);
                            measureNumber++;
                        } // measures loop
                        staffIndex++;
                    } // staves loop

                    prevInstrument = instrument;
                } // instruments loop

                UpdateRender(renderData);
                updateRenderData = false;
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

            UpdateFrameData(frameData);
        }
    }
}

void App::RenderLyric(RenderData& renderData, const Lyric& lyric, float positionY, Measure* measure, Note* note)
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (lyric.fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    else if (lyric.fontWeight == FontWeight::Bold)
        paint.isBold = true;
    float positionX = songData->GetPositionXInSong(note->beatPositionInSong, measure->index);
    renderData.AddText(Text(lyric.text[0].text, positionX, positionY, Paint(lyric.color.color, paint)));
}

void App::RenderDirection(RenderData& renderData, const Direction& direction, float positionY, Measure* measure)
{
    if (!direction.rehearsals.empty())
    {
        Rehearsal rehearsal = direction.rehearsals[0];
        Paint paint = Paint();
        paint.textSize = 16.0f;
        if (rehearsal.fontStyle == FontStyle::Italic)
            paint.isItalic = true;
        else if (rehearsal.fontWeight == FontWeight::Bold)
            paint.isBold = true;
        float positionX = songData->GetPositionXInSong(direction.beatPositionInSong, measure->index);
        renderData.AddText(Text(rehearsal.text.string, positionX, positionY, Paint(rehearsal.color.color, paint)));
    }
    else if (!direction.words.empty())
    {
        Words words = direction.words[0];
        Paint paint = Paint();
        paint.textSize = 16.0f;
        if (words.fontStyle == FontStyle::Italic)
            paint.isItalic = true;
        else if (words.fontWeight == FontWeight::Bold)
            paint.isBold = true;
        float positionX = songData->GetPositionXInSong(direction.beatPositionInSong, measure->index);
        renderData.AddText(Text(words.text.string, positionX, positionY, Paint(words.color.color, paint)));
    }
}

void App::SetViewModelData(ViewModelData viewModelData)
{
    playing = viewModelData.playing;
    playLineBeatPosition = viewModelData.playLineBeatPosition;
    currentMeasure = viewModelData.currentMeasure;
}
