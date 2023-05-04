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

    int measureIndex = (int)startMeasure;
    int measureNumber = (int)startMeasure;
    float measurePosition = systemPositionX;
    int measureRenderCount = 15; // the number of measures that need to be rendered
    int currentMeasureRenderedCount = 0; // the number of measures that have been rendered
    for (int m = (int)startMeasure; m <= endMeasure; m++) {
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
                //measure->measureWidth = 80.0f;
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


            auto measureDataItem = system.systemMeasureData.find(measure->index);

            bool showTimeSignature = measure->showTimeSignature || (system.showBeginningTimeSignature && song->DoesMeasureStartNewSystem(measure->index));
            measure->timeSignature.Render(renderData, showTimeSignature, measureDataItem->second.timeSignaturePositionX + measurePosition, staffPositionY, lineSpacing, staff->lines);

            bool showKeySignature = (measure->showKeySignature || (system.showBeginningKeySignature && song->DoesMeasureStartNewSystem(measure->index)));
            measure->keySignature.Render(renderData, showKeySignature, measureDataItem->second.keySignaturePositionX + measurePosition, lineSpacing, staff->lines, measure->clef, 0.0f, staffPositionY);

            bool showClef = measure->showClef || (system.showBeginningClef && song->DoesMeasureStartNewSystem(measure->index));

            float clefPositionX;
            if (measure->clef.clefChanged)
            {
                clefPositionX = measurePosition - 28.0f;
            }
            else
            {
                clefPositionX = measureDataItem->second.clefPositionX + measurePosition;
            }

            measure->clef.Render(renderData, showClef, clefPositionX, song->displayConstants, staff->lines, 0.0f, staffPositionY);

            // render directions
            for (const Direction &direction: measure->directions) {
                float measurePositionY = staffPositionY;

                direction.Render(renderData, measurePosition, measurePositionY);
            }

            if (!measure->startsMultiMeasureRest)
            {
                int noteIndex = 0;
                for (Note *note: measure->notes) {
                    note->Render(renderData, staff->tablatureDisplayType, measure->CalculateNoteYPositionRelativeToMeasure(noteIndex), staff->lines, { measurePosition, staffPositionY }, measure->measureWidth, measureNumber, lineSpacing, { 0.0f, 0.0f }, noteIndex);
                    //RenderNote(renderData, note, measure, measurePosition, staff, staffPositionY, measure->measureWidth, measureNumber, lineSpacing, 0.0f, 0.0f, noteIndex);
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
                for (Chord &chord : measure->chords) {
                    chord.CalculateChordName();
                    float measurePositionY = staffPositionY;
                    chord.Render(renderData, measurePosition, measurePositionY);
                }

                for (auto tuplet : measure->tuplets)
                {
                    float measurePositionY = staffPositionY;
                    tuplet->Render(renderData, Vec2<float>(measurePosition, measurePositionY));
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

void App::RenderCredits(RenderData& renderData, const MusicDisplayConstants& displayConstants, const std::vector<Credit>& credits, float pageX, float pageY)
{
    for (const Credit& credit : credits)
    {
        credit.Render(renderData, { pageX, pageY }, { song->displayConstants.pageWidth, song->displayConstants.pageHeight });
    }
}