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


            RenderTimeSignature(renderData, measure, system.timeSignaturePositionX + measurePosition, lineSpacing, staff->lines, 0.0f, staffPositionY);

            RenderKeySignature(renderData, measure, system.keySignaturePositionX + measurePosition, lineSpacing,staff->lines, 0.0f,staffPositionY);

            RenderClef(renderData, measure, system.clefPositionX + measurePosition, lineSpacing, staff->lines, 0.0f, staffPositionY);

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

                // rendering note beams
                for (const BeamGroup& beamGroup : measure->beams)
                {
                    for (const Beam& beam : beamGroup.beams)
                    {
                        const float beamDistance = 8.0f;
                        const float hookOffset = 10.0f;
                        float beamYOffset = (beamDistance * float(beam.beamLevel-1));

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

void App::RenderKeySignature(RenderData& renderData, const Measure* measure, float positionX, float ls, int lines, float offsetX, float offsetY)
{
    if (measure->showKeySignature && measure->keySignature.fifths != 0) {
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
                renderData.AddGlyph(SMuFLGlyph(SMuFLID::accidentalSharp, positionX + offsetX, positionY, Paint(0xff000000)));
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
                renderData.AddGlyph(SMuFLGlyph(SMuFLID::accidentalFlat, positionX + offsetX, positionY, Paint(0xff000000)));
                positionX += 8.5f;
            }
        }
        // TODO: add naturals

        //renderData.AddBitmap(RenderBitmap(GetKeySignatureAssetID(measure->keySignature.fifths),positionX, positionY, 1.0f, 1.0f,Paint(0xff000000)));
    }
}

void App::RenderClef(RenderData& renderData, const Measure* measure, float positionX, float ls, int lines, float offsetX, float offsetY)
{
    // clef
    if (measure->showClef) {
        float positionY = measure->GetClefLineYPosition(song->displayConstants, lines) + offsetY; // the bottom line + instYPosition
        SMuFLGlyph glyph = SMuFLGlyph(GetClefSMuFLID(measure->clef, lines), positionX + offsetX, positionY, Paint(0xff000000));
        renderData.AddGlyph(glyph);
        /*float glyphWidth = RenderMeasurement::MeasureGlyph(glyph);
        renderData.AddLine(std::make_shared<Line>(positionX + offsetX, positionY, positionX + offsetX + glyphWidth, positionY, Paint(HeavyBarLinePaint)));
        LOGE("glyphWidth: %f", glyphWidth);*/

        //float positionX = measure->GetClefPositionInMeasure(song->GetMeasureWidth(measure->index)) + measurePosition + offsetX;
        //float positionY = (ls * float(lines - 1)) + offsetY; // the bottom line + instYPosition
        //renderData.AddBitmap(RenderBitmap(GetClefAssetID(measure->clef), positionX,positionY, 1.0f, 1.0f,Paint(0xff000000)));
    }
}

void App::RenderTimeSignature(RenderData& renderData, const Measure* measure, float positionX, float ls, int lines, float offsetX, float offsetY)
{
    // time signature
    if (measure->showTimeSignature and measure->timeSignature.print) {

        int spaces = lines - 1;
        float staffMiddle = ((float)spaces/2.0f) * ls;
        float timeDigitHeight = 10.0f;
        float positionY = (staffMiddle - timeDigitHeight) + offsetY;

        renderData.AddGlyph(
                SMuFLGlyph(GetTimeSignatureSMuFLID(measure->timeSignature.notes),positionX + offsetX, positionY,Paint(0xff000000)));
        positionY += 20.0f;
        renderData.AddGlyph(
                SMuFLGlyph(GetTimeSignatureSMuFLID(measure->timeSignature.noteType),positionX + offsetX, positionY,Paint(0xff000000)));
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
    else if (direction.metronomeMark != nullptr)
    {
        float positionX = direction.metronomeMark->positionX + measureXPosition;
        float positionY = direction.metronomeMark->positionY + measurePositionY;
        //std::string string = direction.metronomeMark->tempo + (char)0xECA6;
        std::string string = " = " + direction.metronomeMark->tempo;
        //std::u16string s = direction.metronomeMark->tempo;
        //LOGE("metronomeMark: %s", string.c_str());

        //std::u16string s;
        //s.append((char16_t)0xECA6);
        std::vector<uint16_t> chars;
        chars.push_back((uint16_t)GetMetronomeNoteSMuFLID(direction.metronomeMark->mainNoteUnit.noteValue));
        if (direction.metronomeMark->mainNoteUnit.isDotted)
            chars.push_back((uint16_t)SMuFLID::metAugmentationDot);

        for (auto& c : string)
        {
            if (c == '\0')
                break;
            chars.push_back(c);
        }

        std::vector<TextSpan> spans;
        Paint glyphPaint = Paint(direction.metronomeMark->color.color);
        glyphPaint.useMusicFont = true;
        glyphPaint.textSize = 40.0f;
        glyphPaint.align = Paint::Align::Left;

        Paint paint = Paint(direction.metronomeMark->color.color);
        paint.isBold = true;
        paint.textSize = 26.0f;
        paint.align = Paint::Align::Left;

        if (direction.metronomeMark->mainNoteUnit.isDotted)
        {
            spans.emplace_back(0, 2, glyphPaint);
            spans.emplace_back(2, 10, paint);
        }
        else
        {
            spans.emplace_back(0, 1, glyphPaint);
            spans.emplace_back(1, 10, paint);
        }

        renderData.AddSpannableText(std::make_shared<SpannableText>(chars, positionX + offsetX, positionY + offsetY, spans, Paint(direction.metronomeMark->color.color)));
        //renderData.AddText(Text(string, positionX + offsetX, positionY + offsetY, Paint(direction.metronomeMark->color.color)));
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

        if (slur.type == StartStopType::Start)
        {
            curveStartX = positionX;
            curveStartY = positionY - 6.0f;
        }
    }

    // note stem
    if (note->noteStem.stemType != NoteStem::StemType::None)
    {
        renderData.AddLine(std::make_shared<Line>(positionX, positionY + note->noteStem.stemStartY, positionX, positionY + note->noteStem.stemEndY, NoteStemPaint));
    }

    // note flag
    if (note->beamData.empty() && !note->isChord)
    {
        bool isUp = false;

        if (note->noteStem.stemType == NoteStem::StemType::Up) {
            isUp = true;
        } else if (note->noteStem.stemType == NoteStem::StemType::Down) {
            isUp = false;
        }

        renderData.AddGlyph(SMuFLGlyph(GetNoteFlagSMuFLID(note->durationType, isUp), positionX, positionY + note->noteStem.stemEndY, Paint(normalColor)));
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
    if (note->noteStem.stemType != NoteStem::StemType::None)
    {
        renderData.AddLine(std::make_shared<Line>(notePositionX + note->noteStem.stemPositionX, notePositionY + note->noteStem.stemStartY, notePositionX + note->noteStem.stemPositionX, notePositionY + note->noteStem.stemEndY, NoteStemPaint));
    }

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