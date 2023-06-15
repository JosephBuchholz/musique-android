#include "NoteChord.h"

#include "../../Exceptions/Exceptions.h"

void NoteChord::Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float rootNotePositionYRelativeToMeasure, float topNotePositionYRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float ls) const
{
    if (notes.empty())
        throw IsEmptyException();

    std::shared_ptr<Note> rootNote = notes[0];
    std::shared_ptr<Note> topNote = notes[notes.size() - 1];

    if (rootNote == nullptr || topNote == nullptr)
        throw IsNullException();

    Vec2<float> position = { rootNote->position.x + measurePosition.x, rootNote->position.y + measurePosition.y };

    for (auto note : notes)
    {
        note->Render(renderData, tabDisplayType, 0.0f, lines, measurePosition, nextMeasurePositionX, ls);
    }

    // rhythm notation
    if (tabDisplayType == TablatureDisplayType::Full)
    {
        noteStem->Render(renderData, position, rootNote->tremoloSingle, rootNote->isGraceNote, rootNote->hasSlash, rootNote->noteHead.GetNoteHeadWidth(renderData.displayConstants));

        if (noteFlag)
            noteFlag->Render(renderData, {position.x + noteStem->stemPositionX, position.y + noteStem->stemEndY});
    }

    Paint ledgerLinePaint;
    ledgerLinePaint.strokeWidth = renderData.displayConstants.legerLineWidth;
    ledgerLinePaint.strokeCap = Paint::Cap::Butt;

    // ledger lines
    float ledgerLineMargin = renderData.displayConstants.ledgerLineMargin * rootNote->sizeFactor;
    float noteHeadWidth = rootNote->noteHead.GetNoteHeadWidth(renderData.displayConstants);
    if (rootNotePositionYRelativeToMeasure >= (float)lines) // ledger lines below staff
    {
        int ledgerLineCount = (int)rootNotePositionYRelativeToMeasure - lines + 1;
        float y = measurePosition.y + ((float)lines * ls);
        for (int i = 0; i < ledgerLineCount; i++)
        {
            renderData.AddLine(std::make_shared<Line>(position.x - ledgerLineMargin, y, position.x + noteHeadWidth + ledgerLineMargin, y, ledgerLinePaint));
            y += 1.0f * ls;
        }
    }

    if (topNotePositionYRelativeToMeasure < 0.0f) // ledger lines above parentStaff
    {
        int ledgerLineCount = abs((int)topNotePositionYRelativeToMeasure);
        float y = measurePosition.y - (1.0f * ls);
        for (int i = 0; i < ledgerLineCount; i++)
        {
            renderData.AddLine(std::make_shared<Line>(position.x - ledgerLineMargin, y, position.x + noteHeadWidth + ledgerLineMargin, y, ledgerLinePaint));
            y -= 1.0f * ls;
        }
    }
}

void NoteChord::RenderDebug(RenderData& renderData) const
{
    /*for (auto note : notes)
    {
        note->RenderDebug(renderData);
    }*/

    boundingBox.Render(renderData);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox.Render(renderData);
#endif
}

void NoteChord::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition)
{
    if (notes.empty())
        throw IsEmptyException();

    notes[0]->UpdateBoundingBox(displayConstants, parentPosition);
    boundingBox = notes[0]->boundingBox;

    for (int i = 1; i < notes.size(); i++)
    {
        notes[i]->UpdateBoundingBox(displayConstants, parentPosition);
        boundingBox = BoundingBox::CombineBoundingBoxes(boundingBox, notes[i]->boundingBox);
    }

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void NoteChord::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines, std::shared_ptr<Measure> measure, bool isMuseScore4)
{
    if (notes.empty())
        throw IsEmptyException();

    std::shared_ptr<Note> rootNote = notes[0];
    std::shared_ptr<Note> topNote = notes[notes.size() - 1];

    if (rootNote == nullptr || topNote == nullptr)
        throw IsNullException();

    noteStem = std::make_shared<NoteStem>();
    noteStem->stemType = rootNote->noteStem->stemType;

    // update note flag
    if ((rootNote->durationType == NoteValue::Eighth || rootNote->durationType == NoteValue::Sixteenth || rootNote->durationType == NoteValue::ThirtySecond) && rootNote->beamData.empty())
    {
        noteFlag = std::make_shared<NoteFlag>();
        float defaultSize = 1.0f;
        if (rootNote->noteSize == NoteSize::Grace)
            defaultSize = displayConstants.graceNoteSize;
        else if (rootNote->noteSize == NoteSize::Cue)
            defaultSize = displayConstants.cueNoteSize;
        noteFlag->noteDuration = rootNote->durationType;
        if (noteStem->stemType == NoteStem::StemType::Up) {
            noteFlag->type = NoteFlag::Type::Up;
        } else if (noteStem->stemType == NoteStem::StemType::Down) {
            noteFlag->type = NoteFlag::Type::Down;
        }
        noteFlag->CalculateAsPaged(displayConstants, defaultSize);
    }

    for (auto note : notes)
    {
        if (note->isRest && note->type == NoteType::Tab && isMuseScore4) // musescore only
        {
            note->defaultPosition.y = INVALID_FLOAT_VALUE;
        }

        note->CalculatePositionAsPaged(displayConstants, staffLines);

        if (note->type == NoteType::Standard && !note->isRest)
            note->position.y = (displayConstants.lineSpacing * measure->CalculateNoteYPositionRelativeToMeasure(note));
    }

    if (rootNote->type == NoteType::Tab)
    {
        // note: all stem positions are relative to the note

        float stemLength = displayConstants.tabLineSpacing * 2.5f;

        if (rootNote->durationType == NoteValue::Half) // shorter stem for half notes
        {
            stemLength *= 2.0f/3.0f;
        }

        stemLength *= rootNote->sizeFactor;

        float topNotePositionY = topNote->position.y - rootNote->position.y;

        noteStem->stemPositionX = rootNote->noteHead.GetCenterPositionX(displayConstants);

        if (noteStem->stemType == NoteStem::StemType::Up)
        {
            noteStem->stemStartY = topNotePositionY - (displayConstants.tabLineSpacing * 0.75f);
            noteStem->stemEndY = noteStem->stemStartY - stemLength;
        }
        else if (noteStem->stemType == NoteStem::StemType::Down)
        {
            noteStem->stemStartY = displayConstants.tabLineSpacing * 0.75f;
            noteStem->stemEndY = noteStem->stemStartY + stemLength;
        }
    }
    else // is standard
    {
        // note: all positions are relative to the root note position

        bool isDoubleNoteStack = false;
        float rootNotePositionX = 0.0f;

        float leftNotesPositionX = 0.0f;
        float rightNotesPositionX = 0.0f;

        for (auto note : notes)
        {
            float notePositionX = note->position.x - rootNote->position.x;

            if (notePositionX != rootNotePositionX)
            {
                isDoubleNoteStack = true;
            }

            if (notePositionX > leftNotesPositionX)
            {
                rightNotesPositionX = notePositionX;
            }
            else if (notePositionX < leftNotesPositionX)
            {
                rightNotesPositionX = leftNotesPositionX;
                leftNotesPositionX = notePositionX;
            }
        }

        float noteWidth = rootNote->noteHead.GetNoteHeadWidth(displayConstants);
        float stemPositionX = 0.0f;

        float notePositionX = 0.0f;
        float notePositionY = 0.0f;

        float topNotePositionY = topNote->position.y - rootNote->position.y;

        float stemStokeWidth = 0.8333f * rootNote->sizeFactor;

        float stemLength = 30.0f * rootNote->sizeFactor;
        if (noteStem->stemType == NoteStem::StemType::Up) {
            if (isDoubleNoteStack)
            {
                noteStem->stemPositionX = rightNotesPositionX - stemStokeWidth / 2.0f;
            }
            else
                noteStem->stemPositionX = notePositionX + noteWidth - stemStokeWidth / 2.0f;

            noteStem->stemStartY = notePositionY;
            noteStem->stemEndY = notePositionY + topNotePositionY - stemLength;
        } else if (noteStem->stemType == NoteStem::StemType::Down) {
            if (isDoubleNoteStack)
            {
                noteStem->stemPositionX = rightNotesPositionX - stemStokeWidth / 2.0f;
            }
            else
                noteStem->stemPositionX = notePositionX + stemStokeWidth / 2.0f;

            noteStem->stemStartY = notePositionY + topNotePositionY;
            noteStem->stemEndY = notePositionY + stemLength;
        }
    }

    rootNote->noteStem = noteStem;
}