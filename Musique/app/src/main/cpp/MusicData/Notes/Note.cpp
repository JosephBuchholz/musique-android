#include "Note.h"

void Note::Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float notePositionRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float measureWidth, int measureNumber, float ls, Vec2<float> mainPosition, int noteIndex, bool isLastMeasureInSystem, Vec2<float> offset) const
{
    // calculate color of the note
    int color = renderData.defaultColor;
    if (isPlaying) {
        color = renderData.playedColor;
    }

    if (isRest) { // is a rest
        if (tabDisplayType == TablatureDisplayType::Full)
            RenderRest(renderData, measurePosition.x, lines, ls, mainPosition.x, mainPosition.y + measurePosition.y);
    } else if (type == NoteType::Tab) // is a tab note
    {
        RenderTabNote(renderData, tabDisplayType, measurePosition.x, measureWidth, lines, ls, mainPosition.x, mainPosition.y + measurePosition.y);
    } else // is a standard note
    {
        float renderPositionX = position.x + measurePosition.x;
        float renderPositionY = position.y + measurePosition.y;

        noteHead.Render(renderData, { renderPositionX + mainPosition.x, renderPositionY + mainPosition.y });

        //renderData.AddGlyph(SMuFLGlyph(GetNoteHeadSMuFLID(durationType),renderPositionX + mainPosition.x, renderPositionY + mainPosition.y, Paint(color)));

        // aug dot
        RenderAugmentationDots(renderData, renderPositionX + mainPosition.x, renderPositionY + mainPosition.y);

        // ledger lines
        if (!isChord)
        {
            Paint ledgerLinePaint;
            ledgerLinePaint.strokeWidth = renderData.displayConstants.legerLineWidth;
            ledgerLinePaint.strokeCap = Paint::Cap::Butt;

            float ledgerLineMargin = renderData.displayConstants.ledgerLineMargin;
            float noteHeadWidth = noteHead.GetNoteHeadWidth(renderData.displayConstants);
            if (notePositionRelativeToMeasure >= (float)lines) // ledger lines below staff
            {
                int ledgerLineCount = (int)notePositionRelativeToMeasure - lines + 1;
                float y = measurePosition.y + ((float)lines * ls);
                for (int i = 0; i < ledgerLineCount; i++)
                {
                    renderData.AddLine(std::make_shared<Line>(renderPositionX - ledgerLineMargin + mainPosition.x, y + mainPosition.y, renderPositionX + noteHeadWidth + ledgerLineMargin + mainPosition.x, y + mainPosition.y, ledgerLinePaint));
                    y += 1.0f * ls;
                }
            }
            if (notePositionRelativeToMeasure < 0.0f) // ledger lines above parentStaff
            {
                int ledgerLineCount = abs((int)notePositionRelativeToMeasure);
                float y = measurePosition.y - (1.0f * ls);
                for (int i = 0; i < ledgerLineCount; i++)
                {
                    renderData.AddLine(std::make_shared<Line>(renderPositionX - ledgerLineMargin + mainPosition.x, y + mainPosition.y, renderPositionX + noteHeadWidth + ledgerLineMargin + mainPosition.x, y + mainPosition.y, ledgerLinePaint));
                    y -= 1.0f * ls;
                }
            }
        }

        RenderTie(renderData, GetCenterPosition(renderData.displayConstants).x + measurePosition.x + mainPosition.x, renderPositionY + mainPosition.y, measurePosition.x + mainPosition.x, measurePosition.y + mainPosition.y, measureWidth);

        if (!isChord)
        {
            noteStem->Render(renderData, { renderPositionX + mainPosition.x, renderPositionY + mainPosition.y }, tremoloSingle, isGraceNote, hasSlash, noteHead.GetNoteHeadWidth(renderData.displayConstants));
        }
        if (noteFlag)
            noteFlag->Render(renderData, { renderPositionX + noteStem->stemPositionX, renderPositionY + noteStem->stemEndY });

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

        // render ornament
        for (auto ornament : ornaments)
        {
            if (ornament != nullptr)
                ornament->Render(renderData, renderPositionX + mainPosition.x + (noteHead.GetNoteHeadWidth(renderData.displayConstants) / 2.0f), measurePosition.y);
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

        fermata->Render(renderData, {position.x + measurePosition.x + centerOffset, measurePosition.y});
    }

    if (glissSlide)
    {
        if (glissSlide->notes.first.get() == this && glissSlide->notes.second != nullptr)
        {
            // TODO: fix: currently assuming that the gliss is only contained in a single measure

            Vec2<float> otherNoteMeasurePosition = measurePosition;

            if (glissSlide->notes.first->measureIndex != glissSlide->notes.second->measureIndex)
            {
                otherNoteMeasurePosition.x = nextMeasurePositionX;
            }

            if (type == NoteType::Standard)
            {
                glissSlide->Render(renderData, {position.x + measurePosition.x + (noteHead.GetNoteHeadWidth(renderData.displayConstants) / 2.0f), position.y + measurePosition.y },
                                   { glissSlide->notes.second->position.x + otherNoteMeasurePosition.x + (glissSlide->notes.second->noteHead.GetNoteHeadWidth(renderData.displayConstants) / 2.0f), glissSlide->notes.second->position.y + otherNoteMeasurePosition.y });
            }
            else
            {
                glissSlide->Render(renderData, {position.x + measurePosition.x, position.y + measurePosition.y }, { glissSlide->notes.second->position.x + otherNoteMeasurePosition.x, glissSlide->notes.second->position.y + otherNoteMeasurePosition.y });
            }

            //glissSlide->Render(renderData, {position.x + measurePosition.x, position.y + measurePosition.y }, { glissSlide->notes.second->position.x + measurePosition.x, glissSlide->notes.second->position.y + measurePosition.y });
        }
    }

    if (tie)
    {
        if (tie->notes.first.get() == this && tie->notes.second != nullptr) // if this note is the start of the tie
        {
            Vec2<float> startNotePosition = measurePosition + GetCenterPosition(renderData.displayConstants);

            Vec2<float> otherNoteMeasurePosition = measurePosition;
            if (tie->notes.first->measureIndex != tie->notes.second->measureIndex) // the notes are in different measures
            {
                otherNoteMeasurePosition.x = nextMeasurePositionX;
            }
            Vec2<float> endNotePosition = otherNoteMeasurePosition + tie->notes.second->GetCenterPosition(renderData.displayConstants);

            tie->Render(renderData, startNotePosition, endNotePosition);
        }
    }

    for (const auto& lyric: lyrics) {
        lyric.Render(renderData, position.x + measurePosition.x, measurePosition.y, mainPosition.x, mainPosition.y);
    } // lyrics loop
}

void Note::RenderDebug(RenderData& renderData) const
{
    boundingBox.Render(renderData, (int)0x660000FF);

    if (fermata)
        fermata->RenderDebug(renderData);

    if (glissSlide)
    {
        if (glissSlide->notes.first.get() == this)
            glissSlide->RenderDebug(renderData);
    }

    for (auto ornament : ornaments)
    {
        if (ornament)
            ornament->RenderDebug(renderData);
    }

    for (auto& lyric : lyrics)
    {
#if DEBUG_BOUNDING_BOXES
        lyric.debugBoundingBox.Render(renderData, (int)0xFF00FF00);
#endif

        lyric.boundingBox.Render(renderData);
    }
}

void Note::RenderRest(RenderData& renderData, float measurePositionX, int lines, float ls, float offsetX, float offsetY) const
{
    // calculate color of the note
    int color = renderData.defaultColor;
    if (isPlaying) {
        color = renderData.playedColor;
    }

    float positionX = position.x + measurePositionX + offsetX;

    int line = (lines / 2) + 1;
    //float linePositionY = position.y + (float(line-1) * ls) + offsetY;
    float linePositionY = position.y + offsetY; // the position of the line that all rests "hang" on (except for the whole rest)
    float positionY = linePositionY;

    if (isFullMeasureRest) {
        positionY -= ls;
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(durationType), positionX, positionY, Paint(color)));
    }
    else if (durationType == NoteValue::Whole) {
        positionY -= ls;
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(durationType), positionX, positionY, Paint(color)));
    }
    else {
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(durationType), positionX, positionY, Paint(color)));
    }
}

void Note::RenderTabNote(RenderData& renderData, TablatureDisplayType tabDisplayType, float measurePositionX, float measureWidth, int lines, float ls, float offsetX, float offsetY) const
{
    // calculate color of the note
    int color = renderData.defaultColor;
    if (isPlaying) {
        color = renderData.playedColor;
    }

    //float position.x = song->GetPositionXInMeasure(beatPositionInSong,measureIndex) + measurePositionX + offsetX; // this line of code crashes the program
    //float position.y = (ls * float(string - 1)) + offsetY;

    float tabPositionX = position.x + measurePositionX + offsetX;
    float tabPositionY = position.y + offsetY;

    noteHead.Render(renderData, { tabPositionX, tabPositionY }, true);

    /*if (noteHead.type == NoteHead::NoteHeadType::X)
    {
        noteHead.Render(renderData, {tabPositionX, tabPositionY}, true);
    }
    else
    {
        renderData.AddText(Text(ToString(fret), tabPositionX, tabPositionY, Paint(color, renderData.paints.tabTextPaint)));
    }*/

    // hammer-ons and pull-offs
    /*for (const TABSlur& slur : tabSlurs) {
        if (slur.type == StartStopType::Stop) {
            CubicCurve curve = CubicCurve();

            // start
            curve.x1 = curveStartX;
            curve.y1 = curveStartY;

            // curve points
            curve.x2 = curveStartX + 10.0f;
            curve.y2 = curveStartY - 10.0f;

            curve.x3 = position.x - 10.0f;
            curve.y3 = curveStartY - 10.0f;

            // end
            curve.x4 = position.x;
            curve.y4 = position.y - 6.0f;

            curve.paint = TabSlurPaint;

            renderData.AddCubicCurve(curve);
        }

        if (slur.type == StartStopType::Start)
        {
            curveStartX = position.x;
            curveStartY = position.y - 6.0f;
        }
    }*/

    // render articulations
    for (auto articulation : articulations)
    {
        if (articulation != nullptr)
            articulation->Render(renderData, tabPositionX, tabPositionY);
    }

    // render techniques
    for (auto technique : techniques)
    {
        if (technique != nullptr)
            technique->Render(renderData, tabPositionX, tabPositionY);
    }

    // render ornaments
    for (auto ornament : ornaments)
    {
        if (ornament != nullptr)
            ornament->Render(renderData, tabPositionX, offsetY);
    }

    // rhythm notation
    if (tabDisplayType == TablatureDisplayType::Full)
    {
        if (!isChord)
        {
            noteStem->Render(renderData, { tabPositionX, tabPositionY }, tremoloSingle, isGraceNote, hasSlash, noteHead.GetNoteHeadWidth(renderData.displayConstants));
        }
        if (noteFlag)
            noteFlag->Render(renderData, { tabPositionX + noteStem->stemPositionX, tabPositionY + noteStem->stemEndY });

        RenderTie(renderData, tabPositionX, tabPositionY, measurePositionX + offsetX, offsetY, measureWidth);

        // aug dot
        RenderAugmentationDots(renderData, tabPositionX, tabPositionY);
    }
}

void Note::RenderTie(RenderData& renderData, float noteCenterPositionX, float notePositionY, float measurePositionX, float measurePositionY, float measureWidth) const
{

    // tie
    /*if (tie.type == NoteTie::TieType::Start && tie.tiedNote != nullptr)
    {
        CubicCurve curve = CubicCurve();

        float direction = 1.0f;

        float offsetY = -10.0f;

        if (tie.orientation == CurveOrientation::Under)
            direction = -1.0f;

        if (tie.placement == AboveBelowType::Below)
            offsetY = 10.0f;


        float startCurveX = noteCenterPositionX;
        float startCurveY = notePositionY + offsetY;

        float endCurveX;
        float endCurveY;

        if (tie.tiedNote->measureIndex > measureIndex) // if the tied note is in the next measure
        {
            // if (song->DoesMeasureStartNewSystem(tie.tiedNote->measureIndex))
            if (false) // if the tied note is on the next system
            {
                endCurveX = measurePositionX + measureWidth;
                endCurveY = tie.tiedNote->position.y + measurePositionY + offsetY;

                // render second tie object
                // TODO: finnish 2nd curve

                //start//float startCurveX2 = noteCenterPositionX;
                float startCurveY2 = tie.tiedNote->position.y + measurePositionY + offsetY;

                float endCurveX2 = tie.tiedNote->GetCenterPositionX() + measurePositionX + measureWidth;
                float endCurveY2 = tie.tiedNote->position.y + measurePositionY + offsetY;

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

                renderData.AddCubicCurve(curve2);//end//
            }
            else
            {
                endCurveX = tie.tiedNote->GetCenterPosition(renderData.displayConstants).x + measurePositionX + measureWidth;
                endCurveY = tie.tiedNote->position.y + measurePositionY + offsetY;
            }
        }
        else
        {
            endCurveX = tie.tiedNote->GetCenterPosition(renderData.displayConstants).x + measurePositionX;
            endCurveY = tie.tiedNote->position.y + measurePositionY + offsetY;
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
    }*/
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
    if (!noteStem)
        noteStem = std::make_shared<NoteStem>();

    // update note head
    noteHead.noteDuration = durationType;
    noteHead.noteType = type;
    noteHead.fret = fret;
    noteHead.CalculateAsPaged(displayConstants, noteSize);

    // update note flag
    if ((durationType == NoteValue::Eighth || durationType == NoteValue::Sixteenth || durationType == NoteValue::ThirtySecond) && !isChord && beamData.empty())
    {
        noteFlag = std::make_shared<NoteFlag>();
        float defaultSize = 1.0f;
        if (noteSize == NoteSize::Grace)
            defaultSize = displayConstants.graceNoteSize;
        else if (noteSize == NoteSize::Cue)
            defaultSize = displayConstants.cueNoteSize;
        noteFlag->noteDuration = durationType;
        if (noteStem->stemType == NoteStem::StemType::Up) {
            noteFlag->type = NoteFlag::Type::Up;
        } else if (noteStem->stemType == NoteStem::StemType::Down) {
            noteFlag->type = NoteFlag::Type::Down;
        }
        noteFlag->CalculateAsPaged(displayConstants, defaultSize);
    }

    accidental.CalculateAsPaged(displayConstants, noteSize);

    if (noteSize == NoteSize::Grace)
        size = displayConstants.graceNoteSize;
    else if (noteSize == NoteSize::Cue)
        size = displayConstants.cueNoteSize;

    if (isRest) // is a rest
    {
        if (defaultPosition.x != INVALID_FLOAT_VALUE)
        {
            position.x = defaultPosition.x;

            if (relativePosition.x != INVALID_FLOAT_VALUE)
                position.x += relativePosition.x;
        }

        if (defaultPosition.y != INVALID_FLOAT_VALUE)
        {
            position.y = -defaultPosition.y;

            if (relativePosition.y != INVALID_FLOAT_VALUE)
                position.y -= relativePosition.y;
        }
        else if (defaultPosition.y == INVALID_FLOAT_VALUE)
        {
            if (type == NoteType::Standard)
            {
                int line = (staffLines / 2) + 1;
                float linePositionY = (float(line - 1) * displayConstants.lineSpacing);
                position.y = linePositionY;
            }
            else if (type == NoteType::Tab)
            {
                int line = (staffLines / 2) + 1;
                float linePositionY = (float(line - 1) * displayConstants.tabLineSpacing);
                position.y = linePositionY;
            }
        }
    }
    else if (type == NoteType::Tab) // is a tab note
    {
        if (defaultPosition.x != INVALID_FLOAT_VALUE)
            position.x = defaultPosition.x;
        if (relativePosition.x != INVALID_FLOAT_VALUE)
            position.x += relativePosition.x;

        position.y = (displayConstants.tabLineSpacing * float(string - 1));

        if (!isChord)
        {
            // note: all stem positions are relative to the note

            float stemLength = displayConstants.tabLineSpacing * 2.5f;

            if (durationType == NoteValue::Half) // shorter stem for half notes
            {
                stemLength *= 2.0f/3.0f;
            }

            stemLength *= size;

            noteStem->stemPositionX = 0.0f;

            if (noteStem->stemType == NoteStem::StemType::Up)
            {
                noteStem->stemStartY = -(displayConstants.tabLineSpacing * 0.75f);
                noteStem->stemEndY = noteStem->stemStartY - stemLength;
            }
            else if (noteStem->stemType == NoteStem::StemType::Down)
            {
                noteStem->stemStartY = displayConstants.tabLineSpacing * 0.75f;
                noteStem->stemEndY = noteStem->stemStartY + stemLength;
            }
        }
    }
    else // is a standard note
    {
        if (defaultPosition.x != INVALID_FLOAT_VALUE && defaultPosition.y != INVALID_FLOAT_VALUE)
        {
            position.x = defaultPosition.x;
            position.y = -defaultPosition.y;

            if (relativePosition.x != INVALID_FLOAT_VALUE)
                position.x += relativePosition.x;

            if (relativePosition.y != INVALID_FLOAT_VALUE)
                position.y -= relativePosition.y;

            //position.x = defX + relX;
            //position.y = -defY + -relY;
        }

        float noteWidth = noteHead.GetNoteHeadWidth(displayConstants);
        float stemPositionX = 0.0f;

        float notePositionX = 0.0f;
        float notePositionY = 0.0f;

        float stemStokeWidth = 0.8333f * size;

        float stemLength = 30.0f * size;
        if (noteStem->stemType == NoteStem::StemType::Up) {
            noteStem->stemPositionX = notePositionX + noteWidth - stemStokeWidth / 2.0f;

            noteStem->stemStartY = notePositionY;
            noteStem->stemEndY = notePositionY - stemLength;
        } else if (noteStem->stemType == NoteStem::StemType::Down) {
            noteStem->stemPositionX = notePositionX + stemStokeWidth / 2.0f;

            noteStem->stemStartY = notePositionY;
            noteStem->stemEndY = notePositionY + stemLength;
        }
    }

    for (auto articulation : articulations)
    {
        if (articulation != nullptr)
            articulation->CalculatePositionAsPaged(displayConstants, position.y, noteHead.GetNoteHeadWidth(displayConstants), type == NoteType::Tab);
    }

    for (auto technique : techniques)
    {
        if (technique != nullptr)
            technique->CalculatePositionAsPaged(displayConstants, position.y, noteHead.GetNoteHeadWidth(displayConstants), type == NoteType::Tab);
    }

    for (auto ornament : ornaments)
    {
        if (ornament != nullptr)
            ornament->CalculatePositionAsPaged(displayConstants, { 0.0f, 0.0f });
    }

    for (auto& dot : dots)
    {
        dot.CalculatePositionAsPaged(displayConstants, ((position.y / displayConstants.lineSpacing) - floor(position.y / displayConstants.lineSpacing)) == 0.0f, type == NoteType::Tab);
    }

    if (fermata)
        fermata->CalculatePositionAsPaged(displayConstants, { 0.0f, -20.0f });

    if (tremoloSingle)
        tremoloSingle->CalculatePositionAsPaged(displayConstants);

    if (glissSlide)
    {
        if (glissSlide->notes.first.get() == this)
        {
            Vec2<float> defPositionStart = { 0.0f, 0.0f };
            Vec2<float> defPositionEnd = { 0.0f, 0.0f };

            float margin = (noteHead.GetNoteHeadWidth(displayConstants) / 2.0f) + 5.0f;

            defPositionStart.x += margin;
            defPositionEnd.x -= margin;

            glissSlide->CalculatePositionAsPaged(displayConstants, defPositionStart, defPositionEnd, (type == NoteType::Tab));
        }
    }

    if (tie)
    {
        tie->CalculatePositionAsPaged(displayConstants, { 0.0f, 0.0f }, { 0.0f, 0.0f });
    }
}

Vec2<float> Note::GetCenterPosition(const MusicDisplayConstants& displayConstants) const
{
    if (type == NoteType::Tab)
        return position;

    return { position.x + (noteHead.GetNoteHeadWidth(displayConstants) / 2.0f), position.y };
}

void Note::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, const Vec2<float> &parentPosition)
{
    noteHead.UpdateBoundingBox(displayConstants, position + parentPosition);

    boundingBox = noteHead.boundingBox;

    if (type == NoteType::Tab)
    {
        boundingBox.position.x -= boundingBox.size.x / 2.0f;
    }

    if (!isChord)
    {
        if (noteStem->stemEndY - noteStem->stemStartY != 0)
            boundingBox.size.y = noteStem->stemEndY - noteStem->stemStartY;
        else
            boundingBox.size.y = 10.0f;
    }
    else
        boundingBox.size.y = 10.0f;

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif

    if (fermata)
        fermata->UpdateBoundingBox({ position.x + parentPosition.x, parentPosition.y });

    if (glissSlide)
    {
        if (glissSlide->notes.first.get() == this)
        {
            glissSlide->UpdateBoundingBox({ 0.0f, 0.0f });
        }
    }

    for (auto ornament : ornaments)
    {
        if (ornament != nullptr)
            ornament->UpdateBoundingBox({ position.x + parentPosition.x, parentPosition.y });
    }

    for (Lyric& lyric : lyrics)
    {
        lyric.UpdateBoundingBox(Vec2<float>{ position.x + parentPosition.x, parentPosition.y });
    }
}