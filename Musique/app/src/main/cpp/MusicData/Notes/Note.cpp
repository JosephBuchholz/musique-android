#include "Note.h"

void Note::Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float notePositionRelativeToMeasure, int lines, Vec2<float> measurePosition, float measureWidth, int measureNumber, float ls, Vec2<float> mainPosition, int noteIndex, Vec2<float> offset) const
{
    // calculate color of the note
    int color = renderData.defaultColor;
    if (isPlaying) {
        color = renderData.playedColor;
    }

    if (isRest) { // is a rest
        if (tabDisplayType == TablatureDisplayType::Full)
            RenderRest(renderData, this, measurePosition.x, lines, ls, mainPosition.x, mainPosition.y + measurePosition.y);
    } else if (type == NoteType::Tab) // is a tab note
    {
        RenderTabNote(renderData, this, tabDisplayType, measurePosition.x, measureWidth, lines, ls, mainPosition.x, mainPosition.y + measurePosition.y);
    } else // is a standard note
    {
        float renderPositionX = positionX + measurePosition.x;
        float renderPositionY = positionY + measurePosition.y;

        noteHead.Render(renderData, { renderPositionX + mainPosition.x, renderPositionY + mainPosition.y });

        //renderData.AddGlyph(SMuFLGlyph(GetNoteHeadSMuFLID(durationType),renderPositionX + mainPosition.x, renderPositionY + mainPosition.y, Paint(color)));

        // aug dot
        RenderAugmentationDots(renderData, renderPositionX + mainPosition.x, renderPositionY + mainPosition.y);

        // ledger lines
        if (notePositionRelativeToMeasure >=
            (float) lines) // ledger lines below staff
        {
            int ledgerLineCount =
                    (int)notePositionRelativeToMeasure -
                            lines + 1;
            float y = measurePosition.y +
                      ((float) lines * ls);
            for (int i = 0; i < ledgerLineCount; i++) {
                renderData.AddLine(std::make_shared<Line>(renderPositionX - 4.0f + mainPosition.x, y + mainPosition.y, renderPositionX + 14.0f + mainPosition.x, y + mainPosition.y, renderData.paints.barlinePaint));
                y += 1.0f * ls;
            }
        }
        if (notePositionRelativeToMeasure <
            0.0f) // ledger lines above parentStaff
        {
            int ledgerLineCount = abs(
                    (int)notePositionRelativeToMeasure);
            float y = measurePosition.y - (1.0f * ls);
            for (int i = 0; i < ledgerLineCount; i++) {
                renderData.AddLine(std::make_shared<Line>(renderPositionX - 4.0f + mainPosition.x, y + mainPosition.y, renderPositionX + 14.0f + mainPosition.x, y + mainPosition.y, renderData.paints.barlinePaint));
                y -= 1.0f * ls;
            }
        }

        /*for (Slur slur: slurs) {
            if (slur.type == StartStopType::Stop) {
                CubicCurve curve = CubicCurve();

                // start
                curve.x1 = curveStartX;
                curve.y1 = curveStartY;

                // curve points
                curve.x2 = curveStartX + 10.0f;
                curve.y2 = curveStartY - 10.0f;

                curve.x3 = renderPositionX - 10.0f;
                curve.y3 = curveStartY - 10.0f;

                // end
                curve.x4 = renderPositionX;
                curve.y4 = renderPositionY - 6.0f;

                curve.paint = TabSlurPaint;

                renderData.AddCubicCurve(curve);
            }

            if (slur.type == StartStopType::Start) {
                curveStartX = renderPositionX;
                curveStartY = renderPositionY - 6.0f;
            }
        }*/

        RenderTie(renderData, this, GetCenterPositionX(renderData.displayConstants) + measurePosition.x + mainPosition.x, renderPositionY + mainPosition.y, measurePosition.x + mainPosition.x, measurePosition.y + mainPosition.y, measureWidth);

        RenderNoteStem(renderData, this, renderPositionX + mainPosition.x, renderPositionY + mainPosition.y);
        RenderNoteFlag(renderData, this, renderPositionX + mainPosition.x, renderPositionY + mainPosition.y);

        accidental.Render(renderData, { renderPositionX + mainPosition.x, renderPositionY + mainPosition.y });

        // render articulations
        for (auto articulation : articulations)
        {
            if (articulation != nullptr)
                articulation->Render(renderData, renderPositionX + mainPosition.x, renderPositionY + mainPosition.y);
        }

        // render techniques
        for (auto technique : techniques)
        {
            if (technique != nullptr)
                technique->Render(renderData, renderPositionX + mainPosition.x, renderPositionY + mainPosition.y);
        }
    }


    if (fermata)
    {
        float centerOffset;
        if (type == NoteType::Standard)
        {
            centerOffset = (noteHead.GetNoteHeadWidth(renderData.displayConstants) / 2.0f);
        }
        else
        {
            centerOffset = 0.0f;
        }

        fermata->Render(renderData, {positionX + measurePosition.x + centerOffset, measurePosition.y});
    }

    for (const auto& lyric: lyrics) {
        lyric.Render(renderData, positionX + measurePosition.x, measurePosition.y, mainPosition.x, mainPosition.y);
    } // lyrics loop
}

void Note::RenderDebug(RenderData& renderData) const
{
    boundingBox.Render(renderData, (int)0x660000FF);

    if (fermata)
        fermata->RenderDebug(renderData);

    for (auto& lyric : lyrics)
    {
#if DEBUG_BOUNDING_BOXES
        lyric.debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif

        lyric.boundingBox.Render(renderData);
    }
}

void Note::RenderRest(RenderData& renderData, const Note* note, float measurePositionX, int lines, float ls, float offsetX, float offsetY) const
{
    // calculate color of the note
    int color = renderData.defaultColor;
    if (note->isPlaying) {
        color = renderData.playedColor;
    }

    float positionX = note->positionX + measurePositionX + offsetX;

    int line = (lines / 2) + 1;
    //float linePositionY = note->positionY + (float(line-1) * ls) + offsetY;
    float linePositionY = note->positionY + offsetY; // the position of the line that all rests "hang" on (except for the whole rest)
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

void Note::RenderTabNote(RenderData& renderData, const Note* note, TablatureDisplayType tabDisplayType, float measurePositionX, float measureWidth, int lines, float ls, float offsetX, float offsetY) const
{
    // calculate color of the note
    int color = renderData.defaultColor;
    if (note->isPlaying) {
        color = renderData.playedColor;
    }

    //float positionX = song->GetPositionXInMeasure(note->beatPositionInSong,note->measureIndex) + measurePositionX + offsetX; // this line of code crashes the program
    //float positionY = (ls * float(note->string - 1)) + offsetY;

    float tabPositionX = note->positionX + measurePositionX + offsetX;
    float tabPositionY = note->positionY + offsetY;

    noteHead.Render(renderData, { tabPositionX, tabPositionY }, true);

    /*if (noteHead.type == NoteHead::NoteHeadType::X)
    {
        noteHead.Render(renderData, {tabPositionX, tabPositionY}, true);
    }
    else
    {
        renderData.AddText(Text(ToString(note->fret), tabPositionX, tabPositionY, Paint(color, renderData.paints.tabTextPaint)));
    }*/

    // hammer-ons and pull-offs
    /*for (const TABSlur& slur : note->tabSlurs) {
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
    }*/

    // render articulations
    for (auto articulation : note->articulations)
    {
        if (articulation != nullptr)
            articulation->Render(renderData, tabPositionX, tabPositionY);
    }

    // render techniques
    for (auto technique : note->techniques)
    {
        if (technique != nullptr)
            technique->Render(renderData, tabPositionX, tabPositionY);
    }

    // rhythm notation
    if (tabDisplayType == TablatureDisplayType::Full)
    {
        RenderNoteStem(renderData, note, tabPositionX, tabPositionY);
        RenderNoteFlag(renderData, note, tabPositionX, tabPositionY);

        RenderTie(renderData, note, tabPositionX, tabPositionY, measurePositionX + offsetX, offsetY, measureWidth);

        // aug dot
        RenderAugmentationDots(renderData, tabPositionX, tabPositionY);
    }
}

void Note::RenderNoteFlag(RenderData& renderData, const Note* note, float notePositionX, float notePositionY) const
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

        Paint paint(renderData.defaultColor);
        if (noteSize == NoteSize::Grace)
            paint.glyphSizeFactor = renderData.displayConstants.graceNoteSize;
        else if (noteSize == NoteSize::Cue)
            paint.glyphSizeFactor = renderData.displayConstants.cueNoteSize;

        renderData.AddGlyph(SMuFLGlyph(GetNoteFlagSMuFLID(note->durationType, isUp), notePositionX + note->noteStem.stemPositionX, notePositionY + note->noteStem.stemEndY, paint));
    }
}

void Note::RenderNoteStem(RenderData& renderData, const Note* note, float notePositionX, float notePositionY) const
{
    // note stem
    if (note->noteStem.stemType != NoteStem::StemType::None)
    {
        renderData.AddLine(std::make_shared<Line>(notePositionX + note->noteStem.stemPositionX, notePositionY + note->noteStem.stemStartY, notePositionX + note->noteStem.stemPositionX, notePositionY + note->noteStem.stemEndY, renderData.paints.noteStemPaint));
    }

    // render grace note slash
    if (isGraceNote && hasSlash)
    {
        Vec2<float> positionStart = { note->noteStem.stemPositionX - (noteHead.GetNoteHeadWidth(renderData.displayConstants) / 2.0f), (note->noteStem.stemEndY - note->noteStem.stemStartY) / 3.0f };
        Vec2<float> positionEnd = { positionStart.x + noteHead.GetNoteHeadWidth(renderData.displayConstants) + 2.0f, positionStart.y + ((note->noteStem.stemEndY - note->noteStem.stemStartY) / 4.0f) };
        renderData.AddLine(std::make_shared<Line>(notePositionX + positionStart.x, notePositionY + positionStart.y + note->noteStem.stemStartY, notePositionX + positionEnd.x, notePositionY + positionEnd.y + note->noteStem.stemStartY, renderData.paints.noteStemPaint));
    }
}

void Note::RenderTie(RenderData& renderData, const Note* note, float noteCenterPositionX, float notePositionY, float measurePositionX, float measurePositionY, float measureWidth) const
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
            // if (song->DoesMeasureStartNewSystem(note->tie.tiedNote->measureIndex))
            if (false) // if the tied note is on the next system
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
                endCurveX = note->tie.tiedNote->GetCenterPositionX(renderData.displayConstants) + measurePositionX + measureWidth;
                endCurveY = note->tie.tiedNote->positionY + measurePositionY + offsetY;
            }
        }
        else
        {
            endCurveX = note->tie.tiedNote->GetCenterPositionX(renderData.displayConstants) + measurePositionX;
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

        curve.paint = renderData.paints.tiePaint;

        renderData.AddCubicCurve(curve);
    }
}

void Note::RenderAugmentationDots(RenderData& renderData, float notePositionX, float notePositionY) const
{
    // aug dot
    for (const auto& dot : dots)
    {
        dot.Render(renderData, notePositionX, notePositionY);
    }
}

bool Note::IsNoteIsHigher(Note* note1, Note* note2)
{
    if (note1->pitch.GetPitchValue() > note2->pitch.GetPitchValue())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Note::OnPlay() {
    isPlaying = true;
}

void Note::OnStop() {
    isPlaying = false;
}

float Note::GetMinWidth() {
    float width = 0.0f;
    width += duration.duration * 5.0f * 10.0f; // should do a bit more calculations here
    if (!lyrics.empty())
        width += 20.0f;
    return width;
}

void Note::CalculateDurationTypeFromString(const std::string& s) {
    if (s == "whole") {
        durationType = NoteValue::Whole;
    } else if (s == "half") {
        durationType = NoteValue::Half;
    } else if (s == "quarter") {
        durationType = NoteValue::Quarter;
    } else if (s == "eighth") {
        durationType = NoteValue::Eighth;
    } else if (s == "16th") {
        durationType = NoteValue::Sixteenth;
    } else if (s == "32nd") {
        durationType = NoteValue::ThirtySecond;
    } else {
        durationType = NoteValue::None;
    }
}

void Note::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines)
{
    noteHead.noteDuration = durationType;
    noteHead.noteType = type;
    noteHead.fret = fret;
    noteHead.CalculateAsPaged(displayConstants, noteSize);

    accidental.CalculateAsPaged(displayConstants, noteSize);

    if (noteSize == NoteSize::Grace)
        size = displayConstants.graceNoteSize;
    else if (noteSize == NoteSize::Cue)
        size = displayConstants.cueNoteSize;

    if (isRest) // is a rest
    {
        if (defaultPosition.x != INVALID_FLOAT_VALUE)
        {
            positionX = defaultPosition.x;

            if (relativePosition.x != INVALID_FLOAT_VALUE)
                positionX += relativePosition.x;
        }

        if (defaultPosition.y != INVALID_FLOAT_VALUE)
        {
            positionY = -defaultPosition.y;

            if (relativePosition.y != INVALID_FLOAT_VALUE)
                positionY -= relativePosition.y;
        }
        else if (defaultPosition.y == INVALID_FLOAT_VALUE)
        {
            if (type == NoteType::Standard)
            {
                int line = (staffLines / 2) + 1;
                float linePositionY = (float(line - 1) * displayConstants.lineSpacing);
                positionY = linePositionY;
            }
            else if (type == NoteType::Tab)
            {
                int line = (staffLines / 2) + 1;
                float linePositionY = (float(line - 1) * displayConstants.tabLineSpacing);
                positionY = linePositionY;
            }
        }
    }
    else if (type == NoteType::Tab) // is a tab note
    {
        if (defaultPosition.x != INVALID_FLOAT_VALUE)
            positionX = defaultPosition.x;
        if (relativePosition.x != INVALID_FLOAT_VALUE)
            positionX += relativePosition.x;

        positionY = (displayConstants.tabLineSpacing * float(string - 1));

        if (!isChord)
        {
            // note: all stem positions are relative to the note

            float stemLength = displayConstants.tabLineSpacing * 2.5f;

            if (durationType == NoteValue::Half) // shorter stem for half notes
            {
                stemLength *= 2.0f/3.0f;
            }

            stemLength *= size;

            noteStem.stemPositionX = 0.0f;

            if (noteStem.stemType == NoteStem::StemType::Up)
            {
                noteStem.stemStartY = -(displayConstants.tabLineSpacing * 0.75f);
                noteStem.stemEndY = noteStem.stemStartY - stemLength;
            }
            else if (noteStem.stemType == NoteStem::StemType::Down)
            {
                noteStem.stemStartY = displayConstants.tabLineSpacing * 0.75f;
                noteStem.stemEndY = noteStem.stemStartY + stemLength;
            }
        }
    }
    else // is a standard note
    {
        if (defaultPosition.x != INVALID_FLOAT_VALUE && defaultPosition.y != INVALID_FLOAT_VALUE)
        {
            positionX = defaultPosition.x;
            positionY = -defaultPosition.y;

            if (relativePosition.x != INVALID_FLOAT_VALUE)
                positionX += relativePosition.x;

            if (relativePosition.y != INVALID_FLOAT_VALUE)
                positionY -= relativePosition.y;

            //positionX = defX + relX;
            //positionY = -defY + -relY;
        }

        float noteWidth = noteHead.GetNoteHeadWidth(displayConstants);
        float stemPositionX = 0.0f;

        float notePositionX = 0.0f;
        float notePositionY = 0.0f;

        float stemStokeWidth = 0.8333f * size;

        float stemLength = 30.0f * size;
        if (noteStem.stemType == NoteStem::StemType::Up) {
            noteStem.stemPositionX = notePositionX + noteWidth - stemStokeWidth / 2.0f;

            noteStem.stemStartY = notePositionY;
            noteStem.stemEndY = notePositionY - stemLength;
        } else if (noteStem.stemType == NoteStem::StemType::Down) {
            noteStem.stemPositionX = notePositionX + stemStokeWidth / 2.0f;

            noteStem.stemStartY = notePositionY;
            noteStem.stemEndY = notePositionY + stemLength;
        }
    }

    for (auto articulation : articulations)
    {
        if (articulation != nullptr)
            articulation->CalculatePositionAsPaged(displayConstants, positionY, noteHead.GetNoteHeadWidth(displayConstants), type == NoteType::Tab);
    }

    for (auto technique : techniques)
    {
        if (technique != nullptr)
            technique->CalculatePositionAsPaged(displayConstants, positionY, noteHead.GetNoteHeadWidth(displayConstants), type == NoteType::Tab);
    }

    for (auto& dot : dots)
    {
        dot.CalculatePositionAsPaged(displayConstants, ((positionY / displayConstants.lineSpacing) - floor(positionY / displayConstants.lineSpacing)) == 0.0f, type == NoteType::Tab);
    }

    if (fermata)
        fermata->CalculatePositionAsPaged(displayConstants, { 0.0f, -20.0f });
}

float Note::GetCenterPositionX(const MusicDisplayConstants& displayConstants) const
{
    if (type == NoteType::Tab)
        return positionX;

    return positionX + (noteHead.GetNoteHeadWidth(displayConstants) / 2.0f);
}

void Note::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, const Vec2<float> &parentPosition)
{
    boundingBox.position.x = positionX + parentPosition.x;
    boundingBox.position.y = positionY + parentPosition.y;
    boundingBox.size.x = noteHead.GetNoteHeadWidth(displayConstants);

    if (type == NoteType::Tab)
    {
        boundingBox.position.x -= boundingBox.size.x / 2.0f;
    }

    if (!isChord)
    {
        if (noteStem.stemEndY - noteStem.stemStartY != 0)
            boundingBox.size.y = noteStem.stemEndY - noteStem.stemStartY;
        else
            boundingBox.size.y = 10.0f;
    }
    else
        boundingBox.size.y = 10.0f;

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif

    if (fermata)
        fermata->UpdateBoundingBox({ positionX + parentPosition.x, parentPosition.y });

    for (Lyric& lyric : lyrics)
    {
        lyric.UpdateBoundingBox(Vec2<float>{ positionX + parentPosition.x, parentPosition.y });
    }
}