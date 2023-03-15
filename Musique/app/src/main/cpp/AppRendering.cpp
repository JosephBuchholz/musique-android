/**
 * This file contains implementations of the rendering functions in App.h
 */

#include "App.h"
#include "Utils/Converters.h"
#include "RenderMeasurement.h"

void App::RenderLineOfMeasures(RenderData& renderData, unsigned int startMeasure, unsigned int endMeasure, const System& system, Staff* staff, float systemPositionX, float staffPositionY, float lineSpacing)
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


            bool showTimeSignature = measure->showTimeSignature || (system.showTimeSignature && song->DoesMeasureStartNewSystem(measure->index));
            measure->timeSignature.Render(renderData, showTimeSignature, system.timeSignaturePositionX + measurePosition, staffPositionY, lineSpacing, staff->lines);

            bool showKeySignature = (measure->showKeySignature || (system.showKeySignature && song->DoesMeasureStartNewSystem(measure->index)));
            measure->keySignature.Render(renderData, showKeySignature, system.keySignaturePositionX + measurePosition, lineSpacing, staff->lines, measure->clef, 0.0f, staffPositionY);

            bool showClef = measure->showClef || (system.showClef && song->DoesMeasureStartNewSystem(measure->index));
            measure->clef.Render(renderData, showClef, system.clefPositionX + measurePosition, song->displayConstants, staff->lines, 0.0f, staffPositionY);

            // render directions
            for (const Direction &direction: measure->directions) {
                float measurePositionY = staffPositionY;

                direction.Render(renderData, measurePosition, measurePositionY);
            }

            if (!measure->startsMultiMeasureRest)
            {
                int noteIndex = 0;
                for (Note *note: measure->notes) {
                    RenderNote(renderData, note, measure, measurePosition, staff, staffPositionY, measure->measureWidth, measureNumber, lineSpacing, 0.0f, 0.0f,
                               noteIndex);
                    noteIndex++;
                }

                // rendering note beams
                for (const BeamGroup& beamGroup : measure->beams)
                {
                    for (const Beam& beam : beamGroup.beams)
                    {
                        const float beamDistance = 8.0f;
                        const float hookOffset = 10.0f;
                        float beamYOffset = (beamDistance * float(beam.beamLevel-1));

                        if (beamGroup.isAboveNote)
                        {
                            beamYOffset = -beamYOffset;
                        }

                        if (beam.beamType == Beam::BeamType::Normal)
                        {
                            renderData.AddLine(std::make_shared<Line>(beam.beamStartPositionX + measurePosition, beam.beamStartPositionY + staffPositionY - beamYOffset, beam.beamEndPositionX + measurePosition, beam.beamEndPositionY + staffPositionY - beamYOffset, NoteBeamPaint));
                        }
                        else if (beam.beamType == Beam::BeamType::ForwardHook)
                        {
                            renderData.AddLine(std::make_shared<Line>(beam.beamStartPositionX + measurePosition, beam.beamStartPositionY + staffPositionY - beamYOffset, beam.beamStartPositionX + measurePosition + hookOffset, beamGroup.GetPositionYOnBeam(beam.beamStartPositionX + hookOffset) + staffPositionY - beamYOffset, NoteBeamPaint));
                        }
                        else if (beam.beamType == Beam::BeamType::BackwardHook)
                        {
                            renderData.AddLine(std::make_shared<Line>(beam.beamStartPositionX + measurePosition, beam.beamStartPositionY + staffPositionY - beamYOffset, beam.beamStartPositionX + measurePosition - hookOffset, beamGroup.GetPositionYOnBeam(beam.beamStartPositionX - hookOffset) + staffPositionY - beamYOffset, NoteBeamPaint));
                        }
                    }
                }

                // render all chords in this measure
                for (Chord &chord: measure->chords) {
                    chord.CalculateChordName();
                    float measurePositionY = staffPositionY;
                    chord.Render(renderData, measurePosition, measurePositionY);
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

    Paint paint = BarLinePaint;
    paint.color = barline.color.color;
    barline.ModifyPaint(paint);

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
    else if (note->durationType == NoteValue::Whole) {
        positionY -= ls;
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(note->durationType), positionX, positionY, Paint(color)));
    }
    else {
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(note->durationType), positionX, positionY, Paint(color)));
    }
}

void App::RenderTabNote(RenderData& renderData, const Note* note, const Staff* staff, float measurePositionX, float measureWidth, int lines, float ls, float offsetX, float offsetY)
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

        if (slur.type == StartStopType::Start)
        {
            curveStartX = positionX;
            curveStartY = positionY - 6.0f;
        }
    }

    // rhythm notation
    if (staff->tablatureDisplayType == Staff::TablatureDisplayType::Full)
    {
        RenderNoteStem(renderData, note, positionX, positionY);
        RenderNoteFlag(renderData, note, positionX, positionY);

        RenderTie(renderData, note, positionX, positionY, measurePositionX + offsetX, offsetY, measureWidth);

        // aug dot
        RenderAugmentationDots(renderData, note->dots, positionX, positionY);
    }
}

void App::RenderNote(RenderData& renderData, const Note* note, Measure* measure, float measurePositionX, const Staff* staff, float measurePositionY, float measureWidth, int measureNumber, float ls, float mainPositionX, float mainPositionY, int noteIndex)
{
    // calculate color of the note
    int color = normalColor;
    if (note->isPlaying) {
        color = playedColor;
    }

    if (note->isRest) { // is a rest
        if (staff->tablatureDisplayType == Staff::TablatureDisplayType::Full)
            RenderRest(renderData, note, measurePositionX, staff->lines, ls, mainPositionX, mainPositionY + measurePositionY);
    } else if (note->type == Note::NoteType::Tab) // is a tab note
    {
        RenderTabNote(renderData, note, staff, measurePositionX, measureWidth, staff->lines, ls, mainPositionX, mainPositionY + measurePositionY);
    } else // is a standard note
    {
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
        RenderAugmentationDots(renderData, note->dots, positionX + mainPositionX, positionY + mainPositionY);

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

        RenderTie(renderData, note, note->GetCenterPositionX() + measurePositionX + mainPositionX, positionY + mainPositionY, measurePositionX + mainPositionX, measurePositionY + mainPositionY, measureWidth);

        RenderNoteStem(renderData, note, positionX + mainPositionX, positionY + mainPositionY);
        RenderNoteFlag(renderData, note, positionX + mainPositionX, positionY + mainPositionY);

        if (note->accidental.accidentalType !=
            Accidental::AccidentalType::None) {
            float accX = positionX - 10.0f;
            float accY = positionY;

            renderData.AddGlyph(SMuFLGlyph(GetAccidentalSMuFLID(note->accidental.accidentalType),accX + mainPositionX, accY + mainPositionY, Paint(0xff000000)));
        }
    }

    for (const auto& lyric: note->lyrics) {
        /*float positionY = (ls * (float) staff->lines) +
                          staffYPosition +
                          instYPosition + 20.0f;*/
        lyric.Render(renderData, note->positionX + measurePositionX, measurePositionY, mainPositionX, mainPositionY);
    } // lyrics loop
}

void App::RenderNoteFlag(RenderData& renderData, const Note* note, float notePositionX, float notePositionY)
{
    // note flag
    if (note->beamData.empty() && !note->isChord)
    {
        bool isUp = false;

        if (note->noteStem.stemType == NoteStem::StemType::Up) {
            isUp = true;
        } else if (note->noteStem.stemType == NoteStem::StemType::Down) {
            isUp = false;
        }

        renderData.AddGlyph(SMuFLGlyph(GetNoteFlagSMuFLID(note->durationType, isUp), notePositionX + note->noteStem.stemPositionX, notePositionY + note->noteStem.stemEndY, Paint(normalColor)));
    }
}

void App::RenderNoteStem(RenderData& renderData, const Note* note, float notePositionX, float notePositionY)
{
    // note stem
    if (note->noteStem.stemType != NoteStem::StemType::None)
    {
        renderData.AddLine(std::make_shared<Line>(notePositionX + note->noteStem.stemPositionX, notePositionY + note->noteStem.stemStartY, notePositionX + note->noteStem.stemPositionX, notePositionY + note->noteStem.stemEndY, NoteStemPaint));
    }
}

void App::RenderTie(RenderData& renderData, const Note* note, float noteCenterPositionX, float notePositionY, float measurePositionX, float measurePositionY, float measureWidth)
{
    // tie
    if (note->tie.type == NoteTie::TieType::Start && note->tie.tiedNote != nullptr)
    {
        CubicCurve curve = CubicCurve();

        float direction = 1.0f;

        float offsetY = -10.0f;

        if (note->tie.orientation == CurveOrientation::Under)
            direction = -1.0f;

        if (note->tie.placement == AboveBelowType::Below)
            offsetY = 10.0f;


        float startCurveX = noteCenterPositionX;
        float startCurveY = notePositionY + offsetY;

        float endCurveX;
        float endCurveY;

        if (note->tie.tiedNote->measureIndex > note->measureIndex) // if the tied note is in the next measure
        {
            if (song->DoesMeasureStartNewSystem(note->tie.tiedNote->measureIndex)) // if the tied note is on the next system
            {
                endCurveX = measurePositionX + measureWidth;
                endCurveY = note->tie.tiedNote->positionY + measurePositionY + offsetY;

                // render second tie object
                // TODO: finnish 2nd curve

                /*float startCurveX2 = noteCenterPositionX;
                float startCurveY2 = note->tie.tiedNote->positionY + measurePositionY + offsetY;

                float endCurveX2 = note->tie.tiedNote->GetCenterPositionX() + measurePositionX + measureWidth;
                float endCurveY2 = note->tie.tiedNote->positionY + measurePositionY + offsetY;

                CubicCurve curve2 = CubicCurve();

                // start
                curve2.x1 = startCurveX2;
                curve2.y1 = startCurveY2;

                // curve points
                curve2.x2 = startCurveX2 + 10.0f;
                curve2.y2 = startCurveY2 - 10.0f * direction;

                curve2.x3 = endCurveX2 - 10.0f;
                curve2.y3 = endCurveY2 - 10.0f * direction;

                // end
                curve2.x4 = endCurveX2;
                curve2.y4 = endCurveY2;

                curve2.paint = TiePaint;

                renderData.AddCubicCurve(curve2);*/
            }
            else
            {
                endCurveX = note->tie.tiedNote->GetCenterPositionX() + measurePositionX + measureWidth;
                endCurveY = note->tie.tiedNote->positionY + measurePositionY + offsetY;
            }
        }
        else
        {
            endCurveX = note->tie.tiedNote->GetCenterPositionX() + measurePositionX;
            endCurveY = note->tie.tiedNote->positionY + measurePositionY + offsetY;
        }

        //renderData.AddDebugDot(startCurveX, startCurveY);
        //renderData.AddDebugDot(endCurveX, endCurveY);

        // start
        curve.x1 = startCurveX;
        curve.y1 = startCurveY;

        // curve points
        curve.x2 = startCurveX + 10.0f;
        curve.y2 = startCurveY - 10.0f * direction;

        curve.x3 = endCurveX - 10.0f;
        curve.y3 = endCurveY - 10.0f * direction;

        // end
        curve.x4 = endCurveX;
        curve.y4 = endCurveY;

        curve.paint = TiePaint;

        renderData.AddCubicCurve(curve);
    }
}

void App::RenderAugmentationDots(RenderData& renderData, const std::vector<AugmentationDot>& dots, float notePositionX, float notePositionY)
{
    // aug dot
    for (const auto& dot : dots)
    {
        dot.Render(renderData, notePositionX, notePositionY);
    }
}

void App::RenderCredits(RenderData& renderData, const MusicDisplayConstants& displayConstants, const std::vector<Credit>& credits, float pageX, float pageY)
{
    for (const Credit& credit : credits)
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

            renderData.AddText(Text(credit.words.text, positionX, positionY, paint));
        }
    }
}