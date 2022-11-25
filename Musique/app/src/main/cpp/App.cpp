#include "App.h"
#include "MusicData/DisplayConstants.h"
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
    DeleteSong();
    LOGI("App Done Deconstructing");
}

void App::LoadSongFromString(const std::string& string)
{
    isUpdating = false;
    playing = false;
    playLineBeatPosition = 0.0f;
    currentMeasure = 0;

    LOGI("Loading song from string");
    if (song != nullptr)
    {
        DeleteSong();
    }

    std::string error = "";
    song = MusicXMLParser::ParseMusicXML(string, error);
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
        isUpdating = true;
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

        if (startRendering) {
            RenderData renderData = RenderData();
            FrameData frameData = FrameData();

            //std::vector<float> measureWidths;

            //for (auto* instrument : song->instruments) {
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
                            float ls = lineSpacing;
                            if (staff->type == Staff::StaffType::Tab) {
                                ls = tabLineSpacing;
                            }

                            // staff y position
                            float staffYPosition = 0.0f;
                            if (staffIndex == 0) {
                                staffYPosition = 0.0f;
                            } else if (staffIndex == 1) {
                                staffYPosition = instrument->staves[staffIndex-1]->GetMiddleHeight(ls, 0, staff->measures.size()) +
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
                                        renderData.AddLine(
                                                Line(measurePosition,
                                                     (ls * i) + staffYPosition + instYPosition,
                                                     endX,
                                                     (ls * i) + staffYPosition + instYPosition,
                                                     BarLinePaint));
                                    }


                                    // measure lines (bar lines)
                                    float x = measurePosition;
                                    renderData.AddLine(
                                            Line(x, 0.0f + staffYPosition + instYPosition, x,
                                                 (ls * float(staff->lines - 1)) + staffYPosition +
                                                 instYPosition, BarLinePaint));
                                    x += song->GetMeasureWidth(measureNumber);
                                    renderData.AddLine(
                                            Line(x, 0.0f + staffYPosition + instYPosition, x,
                                                 (ls * float(staff->lines - 1)) + staffYPosition +
                                                 instYPosition, BarLinePaint));

                                    RenderTimeSignature(renderData, measure, measurePosition, ls, 0.0f, instYPosition + staffYPosition);
                                    RenderClef(renderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);
                                    RenderKeySignature(renderData, measure, measurePosition, ls, staff->lines, 0.0f, instYPosition + staffYPosition);

                                    // render directions
                                    for (const Direction& direction : measure->directions)
                                    {
                                        float positionY = staffYPosition + instYPosition - 20.0f;
                                        RenderDirection(renderData, direction, positionY, measure, 0.0f, 0.0f);
                                    }

                                    int noteIndex = 0;
                                    for (Note *note : measure->notes) {
                                        RenderNote(renderData, note, measure, staff, staffYPosition, instYPosition, measureNumber, ls, 0.0f, 0.0f, noteIndex);
                                        noteIndex++;
                                    }

                                    // render all chords in this measure
                                    for (Chord& chord : measure->chords) {
                                        chord.CalculateChordName();
                                        float positionY = staffYPosition +
                                                          instYPosition - 40.0f;
                                        RenderChord(renderData, chord, positionY, measure, 0.0f, 0.0f);
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

                UpdateRender(renderData);
                updateRenderData = false;
            }

            playLinePosition = song->GetPositionXInSong(playLineBeatPosition, currentMeasure);

            renderData.AddLine(
                    Line(0.0f, 20.0f, 0.0f, instYPosition, BarLinePaint));

            if (playLineHeight == 0.0f) {
                playLineHeight = instYPosition + 80.0f;
            }

            frameData.playLinePosition = playLinePosition;
            frameData.playLinePositionY = playLineY;
            frameData.playLineHeight = playLineHeight;
            frameData.playProgress = playLineBeatPosition / song->totalBeatWidth;

            UpdateFrameData(frameData);
        }
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
                renderData.AddBitmap(RenderBitmap(AssetID::Sharp, positionX, positionY, 1.0f, 1.0f,Paint(0xff000000)));
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
                renderData.AddBitmap(RenderBitmap(AssetID::Flat, positionX, positionY, 1.0f, 1.0f,Paint(0xff000000)));
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
        float positionY = (ls * float(lines - 1)) + offsetY; // the bottom line + instYPosition
        renderData.AddBitmap(RenderBitmap(GetClefAssetID(measure->clef), positionX,positionY, 1.0f, 1.0f,Paint(0xff000000)));
    }
}

void App::RenderTimeSignature(RenderData& renderData, const Measure* measure, float measurePosition, float ls, float offsetX, float offsetY)
{
    // time signature
    if (measure->showTimeSignature and measure->timeSignature.print) {
        float positionX = measure->GetTimeSignaturePositionInMeasure(song->GetMeasureWidth(measure->index)) + measurePosition + offsetX;
        float positionY = 20.0f + offsetY;
        renderData.AddBitmap(
                RenderBitmap(
                        GetTimeSignatureAssetID(
                                measure->timeSignature.notes),
                        positionX, positionY, 1.0f, 1.0f,
                        Paint(0xff000000)));
        positionY = 40.0f + offsetY;
        renderData.AddBitmap(
                RenderBitmap(GetTimeSignatureAssetID(
                                     measure->timeSignature.noteType), positionX,
                             positionY,
                             1.0f, 1.0f,
                             Paint(0xff000000)));
    }
}

void App::RenderLyric(RenderData& renderData, const Lyric& lyric, float positionY, const Measure* measure, const Note* note, float offsetX, float offsetY)
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (lyric.fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    else if (lyric.fontWeight == FontWeight::Bold)
        paint.isBold = true;
    float positionX = song->GetPositionXInSong(note->beatPositionInSong, measure->index);
    renderData.AddText(Text(lyric.text[0].text, positionX + offsetX, positionY + offsetY, Paint(lyric.color.color, paint)));
}

void App::RenderDirection(RenderData& renderData, const Direction& direction, float positionY, Measure* measure, float offsetX, float offsetY)
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
        float positionX = song->GetPositionXInSong(direction.beatPositionInSong, measure->index);
        renderData.AddText(Text(rehearsal.text.string, positionX + offsetX, positionY + offsetY, Paint(rehearsal.color.color, paint)));
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
        float positionX = song->GetPositionXInSong(direction.beatPositionInSong, measure->index);
        renderData.AddText(Text(words.text.string, positionX + offsetX, positionY + offsetY, Paint(words.color.color, paint)));
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
            Line(startX + offsetX,
                 startY + offsetY,
                 endX + offsetX,
                 endY + offsetY,
                 paint));
}

void App::RenderRest(RenderData& renderData, const Note* note, int lines, float ls, float offsetX, float offsetY)
{
    // calculate color of the note
    int color = normalColor;
    if (note->isPlaying) {
        color = playedColor;
    }

    // rendering rest
    float positionX = song->GetPositionXInSong(note->beatPositionInSong, note->measureIndex) + offsetX;
    float positionY = ((ls * float(lines - 1)) / 2.0f) + offsetY;

    if (note->isFullMeasureRest) {
        positionY =
                ls + offsetY;
        renderData.AddBitmap(
                RenderBitmap(AssetID::WholeRest,positionX, positionY, 1.0f, 1.0f,Paint(color)));
    } else if (note->durationType == Note::NoteDurationType::Whole) {
        positionY =
                ls + offsetY;
        renderData.AddBitmap(
                RenderBitmap(AssetID::WholeRest,positionX, positionY, 1.0f, 1.0f,Paint(color)));
    } else if (note->durationType == Note::NoteDurationType::Half) {
        positionY = (ls * 2.0f) + offsetY;
        renderData.AddBitmap(
                RenderBitmap(AssetID::HalfRest,positionX, positionY, 1.0f, 1.0f,Paint(color)));
    } else {
        AssetID id;
        switch (note->durationType) {
            case Note::NoteDurationType::Quarter:
                id = AssetID::QuarterRest;
                break;
            case Note::NoteDurationType::Eighth:
                id = AssetID::EightRest;
                break;
            case Note::NoteDurationType::Sixteenth:
                id = AssetID::SixteenthRest;
                break;
            case Note::NoteDurationType::ThirtySecond:
                id = AssetID::ThirtySecondRest;
                break;
            default:
                id = AssetID::None;
                break;
        }
        renderData.AddBitmap(RenderBitmap(id, positionX, positionY, 1.0f, 1.0f,Paint(color)));
    }
}

void App::RenderTabNote(RenderData& renderData, const Note* note, int lines, float ls, float offsetX, float offsetY)
{
    // calculate color of the note
    int color = normalColor;
    if (note->isPlaying) {
        color = playedColor;
    }

    float positionX = song->GetPositionXInSong(note->beatPositionInSong,note->measureIndex) + offsetX; // this line of code crashes the program
    float positionY = (ls * float(note->string - 1)) + offsetY;
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

void App::RenderChord(RenderData& renderData, const Chord& chord, float positionY, const Measure* measure, float offsetX, float offsetY)
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (chord.fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    else if (chord.fontWeight == FontWeight::Bold)
        paint.isBold = true;
    float positionX = song->GetPositionXInSong(chord.beatPositionInSong, measure->index);
    //LOGE("Chord (%s) is at %f, %f", chord.chordName.string.c_str(), chord.beatPosition, positionX);
    renderData.AddText(Text(chord.chordName.string, positionX + offsetX, positionY + offsetY, Paint(chord.color.color, paint)));
}

void App::RenderNote(RenderData& renderData, const Note* note, Measure* measure, const Staff* staff, float staffYPosition, float instYPosition, int measureNumber, float ls, float mainPositionX, float mainPositionY, int noteIndex)
{
    // calculate color of the note
    int color = normalColor;
    if (note->isPlaying) {
        color = playedColor;
    }

    if (note->isRest) { // is a rest
        RenderRest(renderData, note, staff->lines, ls, mainPositionX, mainPositionY + staffYPosition + instYPosition);
    } else if (note->type ==
               Note::NoteType::Tab) // is a tab note
    {
        RenderTabNote(renderData, note, staff->lines, ls, mainPositionX, mainPositionY + staffYPosition + instYPosition);
    } else // is a standard note
    {
        // rendering note head
        //float positionX = measure->GetNotePositionInMeasure(
        //        song->GetMeasureWidth(measureNumber), noteIndex) + measurePosition;
        float positionX = song->GetPositionXInSong(note->beatPositionInSong, measureNumber);
        float positionY = (ls * measure->GetNoteYPosition(noteIndex)) + staffYPosition + instYPosition;

        renderData.AddBitmap(RenderBitmap(GetNoteHeadAssetID(note->durationType),positionX + mainPositionX, positionY + mainPositionY, 1.0f, 1.0f, Paint(color)));

        // ledger lines
        if (measure->GetNoteYPosition(noteIndex) >=
            (float) staff->lines) // ledger lines below staff
        {
            int ledgerLineCount =
                    (int) measure->GetNoteYPosition(noteIndex) -
                    staff->lines + 1;
            float y = staffYPosition + instYPosition +
                      ((float) staff->lines * ls);
            for (int i = 0; i < ledgerLineCount; i++) {
                renderData.AddLine(
                        Line(positionX - 4.0f + mainPositionX, y + mainPositionY, positionX + 14.0f + mainPositionX, y + mainPositionY,
                             BarLinePaint));
                y += 1.0f * ls;
            }
        }
        if (measure->GetNoteYPosition(noteIndex) <
            0.0f) // ledger lines above staff
        {
            int ledgerLineCount = abs(
                    (int) measure->GetNoteYPosition(noteIndex));
            float y = staffYPosition + instYPosition - (1.0f * ls);
            for (int i = 0; i < ledgerLineCount; i++) {
                renderData.AddLine(
                        Line(positionX - 4.0f + mainPositionX, y + mainPositionY, positionX + 14.0f + mainPositionX, y + mainPositionY,
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

            renderData.AddBitmap(RenderBitmap(
                    GetAccidentalAssetID(
                            note->accidental.accidentalType),
                    accX + mainPositionX, accY + mainPositionY, 1.0f, 1.0f, Paint(0xff000000)));
        }
    }

    for (const auto &lyric: note->lyrics) {
        float positionY = (ls * (float) staff->lines) +
                          staffYPosition +
                          instYPosition + 20.0f;
        RenderLyric(renderData, lyric, positionY, measure, note, mainPositionX, mainPositionY);
    } // lyrics loop
}

void App::RenderNoteStemAndFlagAndBeam(RenderData& renderData, const Note* note, float notePositionX, float notePositionY)
{
    // rendering note stem
    float noteWidth = 11.3f;
    float stemEndY = 0.0f;

    float stemLength = 30.0f;
    if (note->noteStem.stemType == NoteStem::StemType::Up) {
        float px = notePositionX + noteWidth - NoteStemPaint.strokeWidth / 2.0f;
        renderData.AddLine(Line(px, notePositionY, px, notePositionY - stemLength, NoteStemPaint));
        stemEndY = notePositionY - stemLength;
    } else if (note->noteStem.stemType == NoteStem::StemType::Down) {
        float px = notePositionX + NoteStemPaint.strokeWidth / 2.0f;
        renderData.AddLine(Line(px, notePositionY, px, notePositionY + stemLength, NoteStemPaint));
        stemEndY = notePositionY + stemLength;
    } else if (note->noteStem.stemType == NoteStem::StemType::Double) {
        float px = notePositionX + NoteStemPaint.strokeWidth / 2.0f;
        renderData.AddLine(Line(px, notePositionY, px, notePositionY + stemLength,NoteStemPaint));
        px = notePositionX + noteWidth - NoteStemPaint.strokeWidth / 2.0f;
        renderData.AddLine(Line(px, notePositionY, px, notePositionY - stemLength, NoteStemPaint));
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

        renderData.AddLine(Line(beamStartX, beamStartY, beamEndX,beamEndY,NoteBeamPaint));
    }
}

void App::RenderMusicToPage(RenderData& renderData, int page) {
    // --- MAIN RENDERING ---

    float pageWidth = 1360.0f;
    float pageHeight = 1760.0f;

    float leftMargin = 80.0f;
    float rightMargin = 80.0f;
    float topMargin = 80.0f;
    float bottomMargin = 80.0f;

    float mainPositionX = leftMargin;
    float mainPositionY = topMargin;

    /*
     * An instrument's Y position starts at the first staff line (does not include ledger lines)
     * An instrument's middleHeight is the amount of y space that it takes up below it's y position to the last staff line
     * An instrument's aboveHeight is the amount of y space that it takes up above it's y position
     * An instrument's belowHeight is the amount of y space that it takes up below it's y position + middleHeight
     * An instrument's height is aboveHeight + middleHeight + belowHeight
     */

    float instYPosition = 0.0f;
    if (true) {
        Instrument *prevInstrument = nullptr;
        for (auto *instrument: song->instruments) {

            if (prevInstrument != nullptr)
                instYPosition += prevInstrument->GetMiddleHeight(10.0f, 13.33f, 0,
                                                                 instrument->GetMeasureCount()) +
                                 prevInstrument->GetBelowHeight(10.0f, 13.33f, 0,
                                                                instrument->GetMeasureCount());
            instYPosition += instrument->GetAboveHeight(10.0f, 13.33f, 0,
                                                        instrument->GetMeasureCount());

            int staffIndex = 0;
            for (auto *staff: instrument->staves) {

                float ls = lineSpacing;
                if (staff->type == Staff::StaffType::Tab) {
                    ls = tabLineSpacing;
                } else {
                }

                // staff y position
                float staffYPosition = 0.0f;
                if (staffIndex == 0) {
                    staffYPosition = 0.0f;
                } else if (staffIndex == 1) {
                    staffYPosition =
                            instrument->staves[staffIndex - 1]->GetMiddleHeight(ls, 0,
                                                                                staff->measures.size()) +
                            instrument->staves[staffIndex - 1]->GetBelowHeight(ls, 0,
                                                                               staff->measures.size()) +
                            staff->GetAboveHeight(ls, 0, staff->measures.size());
                }

                int measureNumber = 0;
                float measurePosition = 0.0f;
                int measureRenderCount = 15; // the number of measures that need to be rendered
                int currentMeasureRenderedCount = 0; // the number of measures that have been rendered
                for (auto *measure: staff->measures) {

                    if (!(currentMeasureRenderedCount >= measureRenderCount) &&
                        measureNumber >= currentMeasure - (measureRenderCount /
                                                           2)) // render measure in a radius of measureRenderCount/2
                    {
                        float measureWidth = song->GetMeasureWidth(measureNumber);

                        // staff lines
                        for (int i = 0; i < staff->lines; i++) {
                            float endX = measurePosition + measureWidth;
                            AddLine(renderData, measurePosition, ls * i, endX + mainPositionX, ls * i, mainPositionX, staffYPosition + instYPosition + mainPositionY, BarLinePaint);
                            /*renderData.AddLine(
                                    Line(measurePosition + mainPositionX,
                                         (lineSpacing * i) + staffYPosition + instYPosition + mainPositionY,
                                         endX + mainPositionX,
                                         (lineSpacing * i) + staffYPosition + instYPosition + mainPositionY,
                                         BarLinePaint));*/
                        }


                        // measure lines (bar lines)
                        float x = measurePosition;
                        //if (measureNumber != 0) {
                        //    x = measureWidths[measureNumber];
                        //}
                        renderData.AddLine(
                                Line(x + mainPositionX, 0.0f + staffYPosition + instYPosition + mainPositionY, x + mainPositionX,
                                     (ls * float(staff->lines - 1)) + staffYPosition +
                                     instYPosition + mainPositionY, BarLinePaint));
                        x += song->GetMeasureWidth(measureNumber);
                        renderData.AddLine(
                                Line(x + mainPositionX, 0.0f + staffYPosition + instYPosition + mainPositionY, x + mainPositionX,
                                     (ls * float(staff->lines - 1)) + staffYPosition +
                                     instYPosition + mainPositionY, BarLinePaint));

                        /*if (measureNumber == currentMeasure) {
                            // calculating play line position in measure
                            playLinePosInMeasure = measure->GetPlayLinePositionInMeasure(
                                    playLineBeatPosition - currentMeasureBeatPosition,
                                    song->GetMeasureWidth(measureNumber));
                            playLinePosition = measurePosition + playLinePosInMeasure;
                        }*/

                        RenderTimeSignature(renderData, measure, measurePosition, ls, 0.0f + mainPositionX, instYPosition + staffYPosition + mainPositionY);
                        RenderClef(renderData, measure, measurePosition, ls, staff->lines, 0.0f + mainPositionX, instYPosition + staffYPosition + mainPositionY);
                        RenderKeySignature(renderData, measure, measurePosition, ls, staff->lines, 0.0f + mainPositionX, instYPosition + staffYPosition + mainPositionY);

                        // render directions
                        for (const Direction &direction: measure->directions) {
                            float positionY = staffYPosition + instYPosition - 20.0f;
                            RenderDirection(renderData, direction, positionY, measure, mainPositionX, mainPositionY);
                        }

                        int noteIndex = 0;
                        for (Note *note: measure->notes) {
                            RenderNote(renderData, note, measure, staff, staffYPosition, instYPosition, measureNumber, ls, mainPositionX, mainPositionY, noteIndex);
                            noteIndex++;
                        }

                        currentMeasureRenderedCount++;
                    }
                    measurePosition += song->GetMeasureWidth(measureNumber);
                    measureNumber++;
                } // measures loop
                staffIndex++;
            } // staves loop

            prevInstrument = instrument;
        } // instruments loop

    }
}

bool App::OnUpdatePrintLayout()
{
    LOGW("ON UPDATE PRINT LAYOUT!!");
    PrintRenderData printRenderData = PrintRenderData();
    bool layoutChanged = true;

    int totalPages = 1;
    int pageNum = 0;
    while (pageNum < totalPages)
    {
        RenderData pageRenderData = RenderData();
        RenderMusicToPage(pageRenderData, pageNum);
        printRenderData.pages.push_back(pageRenderData);
        pageNum++;
    }

    UpdatePrintRenderData(printRenderData);
    return layoutChanged;
}

int App::OnCalculateNumPages()
{
    LOGW("ON CALCULATE NUM PAGES!!");
    return 1;
}

void App::UpdateInstrumentInfo(const InstrumentInfo& info, unsigned int index)
{
    if (song && song->songData.instrumentInfos.size() > index) {
        if (song->songData.instrumentInfos[index].visible != info.visible) // visible field changed, so update render
            updateRenderData = true;
        song->songData.instrumentInfos[index] = info;
    }
}
