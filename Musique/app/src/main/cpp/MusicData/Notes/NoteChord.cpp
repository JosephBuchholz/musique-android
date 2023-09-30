#include "NoteChord.h"

#include "../../Exceptions/Exceptions.h"

void NoteChord::Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float rootNotePositionYRelativeToMeasure, float topNotePositionYRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float ls) const
{
    if (m_notes.empty())
        throw IsEmptyException();

    std::shared_ptr<Note> rootNote = m_notes[0];
    std::shared_ptr<Note> topNote = m_notes[m_notes.size() - 1];

    if (rootNote == nullptr || topNote == nullptr)
        throw IsNullException();

    Vec2<float> position = { rootNote->position.x + measurePosition.x, rootNote->position.y + measurePosition.y };

    for (auto note : m_notes)
    {
        note->Render(renderData, tabDisplayType, 0.0f, lines, measurePosition, nextMeasurePositionX, ls);
    }

    NoteStem noteStem = rootNote->noteStem;
    // rhythm notation
    if (tabDisplayType == TablatureDisplayType::Full)
    {
        noteStem.Render(renderData, position, rootNote->tremoloSingle, rootNote->isGraceNote, rootNote->hasSlash, rootNote->noteHead.GetNoteHeadWidth(renderData.displayConstants));

        m_noteFlag.Render(renderData, {position.x + noteStem.stemPositionX, position.y + noteStem.stemEndY});
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
            renderData.AddLine(Line({ position.x - ledgerLineMargin, y }, { position.x + noteHeadWidth + ledgerLineMargin, y }, ledgerLinePaint));
            y += 1.0f * ls;
        }
    }

    if (topNotePositionYRelativeToMeasure < 0.0f) // ledger lines above parentStaff
    {
        int ledgerLineCount = abs((int)topNotePositionYRelativeToMeasure);
        float y = measurePosition.y - (1.0f * ls);
        for (int i = 0; i < ledgerLineCount; i++)
        {
            renderData.AddLine(Line({ position.x - ledgerLineMargin, y }, { position.x + noteHeadWidth + ledgerLineMargin, y }, ledgerLinePaint));
            y -= 1.0f * ls;
        }
    }
}

void NoteChord::RenderDebug(RenderData& renderData, TablatureDisplayType tabDisplayType, float rootNotePositionYRelativeToMeasure, float topNotePositionYRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float ls) const
{
    if (m_notes.empty())
        throw IsEmptyException();

    std::shared_ptr<Note> rootNote = m_notes[0];
    std::shared_ptr<Note> topNote = m_notes[m_notes.size() - 1];

    if (rootNote == nullptr || topNote == nullptr)
        throw IsNullException();

    Vec2<float> position = { rootNote->position.x + measurePosition.x, rootNote->position.y + measurePosition.y };

    for (auto note : m_notes)
    {
        note->RenderDebug(renderData, tabDisplayType, 0.0f, lines, measurePosition, nextMeasurePositionX, ls);
    }

    // rhythm notation
    /*if (tabDisplayType == TablatureDisplayType::Full)
    {
        noteStem->Render(renderData, position, rootNote->tremoloSingle, rootNote->isGraceNote, rootNote->hasSlash, rootNote->noteHead.GetNoteHeadWidth(renderData.displayConstants));

        if (m_noteFlag)
            m_noteFlag->Render(renderData, {position.x + noteStem->stemPositionX, position.y + noteStem->stemEndY});
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
    }*/
}

void NoteChord::RenderDebug(RenderData& renderData) const
{
    /*for (auto note : m_notes)
    {
        note->RenderDebug(renderData);
    }*/

    boundingBox.Render(renderData);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox.Render(renderData);
#endif
}

BoundingBox NoteChord::GetBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const
{
    if (m_notes.empty())
        throw IsEmptyException();

    BoundingBox bb = m_notes[0]->GetBoundingBoxRelativeToMeasure(displayConstants);

    for (int i = 1; i < m_notes.size(); i++)
    {
        bb = BoundingBox::CombineBoundingBoxes(bb, m_notes[i]->GetBoundingBoxRelativeToMeasure(displayConstants));
    }

    return bb;
}

BoundingBox NoteChord::GetTotalBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const
{
    if (m_notes.empty())
        throw IsEmptyException();

    BoundingBox bb = m_notes[0]->GetTotalBoundingBoxRelativeToMeasure(displayConstants);

    for (int i = 1; i < m_notes.size(); i++)
    {
        bb = BoundingBox::CombineBoundingBoxes(bb, m_notes[i]->GetTotalBoundingBoxRelativeToMeasure(displayConstants));
    }

    return bb;
}

void NoteChord::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition)
{
    if (m_notes.empty())
        throw IsEmptyException();

    m_notes[0]->UpdateBoundingBox(displayConstants, parentPosition);
    boundingBox = m_notes[0]->boundingBox;

    for (int i = 1; i < m_notes.size(); i++)
    {
        m_notes[i]->UpdateBoundingBox(displayConstants, parentPosition);
        boundingBox = BoundingBox::CombineBoundingBoxes(boundingBox, m_notes[i]->boundingBox);
    }

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void NoteChord::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines, std::shared_ptr<Measure> measure, bool isMuseScore4)
{
    if (m_notes.empty())
        throw IsEmptyException();

    std::shared_ptr<Note> rootNote = m_notes[0];
    std::shared_ptr<Note> topNote = m_notes[m_notes.size() - 1];

    if (rootNote == nullptr || topNote == nullptr)
        throw IsNullException();

    m_noteStem.stemType = rootNote->noteStem.stemType;

    // update note flag
    if ((rootNote->durationType == NoteValue::Eighth || rootNote->durationType == NoteValue::Sixteenth || rootNote->durationType == NoteValue::ThirtySecond) && rootNote->beamData.empty())
    {
        float defaultSize = 1.0f;
        if (rootNote->noteSize == NoteSize::Grace)
            defaultSize = displayConstants.graceNoteSize;
        else if (rootNote->noteSize == NoteSize::Cue)
            defaultSize = displayConstants.cueNoteSize;
        m_noteFlag.noteDuration = rootNote->durationType;
        if (m_noteStem.stemType == NoteStem::StemType::Up) {
            m_noteFlag.type = NoteFlag::Type::Up;
        } else if (m_noteStem.stemType == NoteStem::StemType::Down) {
            m_noteFlag.type = NoteFlag::Type::Down;
        }
        m_noteFlag.CalculateAsPaged(displayConstants, defaultSize);
    }
    else
    {
        m_noteFlag.type = NoteFlag::Type::None;
    }

    for (auto note : m_notes)
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

        if (rootNote->durationType == NoteValue::Half) // shorter stem for half m_notes
        {
            stemLength *= 2.0f/3.0f;
        }

        stemLength *= rootNote->sizeFactor;

        float topNotePositionY = topNote->position.y - rootNote->position.y;

        m_noteStem.stemPositionX = rootNote->noteHead.GetCenterPositionX(displayConstants);

        if (m_noteStem.stemType == NoteStem::StemType::Up)
        {
            m_noteStem.stemStartY = topNotePositionY - (displayConstants.tabLineSpacing * 0.75f);
            m_noteStem.stemEndY = m_noteStem.stemStartY - stemLength;
        }
        else if (m_noteStem.stemType == NoteStem::StemType::Down)
        {
            m_noteStem.stemStartY = displayConstants.tabLineSpacing * 0.75f;
            m_noteStem.stemEndY = m_noteStem.stemStartY + stemLength;
        }
    }
    else // is standard
    {
        // note: all positions are relative to the root note position

        bool isDoubleNoteStack = false;
        float rootNotePositionX = 0.0f;

        float leftNotesPositionX = 0.0f;
        float rightNotesPositionX = 0.0f;

        for (auto note : m_notes)
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
        if (m_noteStem.stemType == NoteStem::StemType::Up) {
            if (isDoubleNoteStack)
            {
                m_noteStem.stemPositionX = rightNotesPositionX - stemStokeWidth / 2.0f;
            }
            else
                m_noteStem.stemPositionX = notePositionX + noteWidth - stemStokeWidth / 2.0f;

            m_noteStem.stemStartY = notePositionY;
            m_noteStem.stemEndY = notePositionY + topNotePositionY - stemLength;
        } else if (m_noteStem.stemType == NoteStem::StemType::Down) {
            if (isDoubleNoteStack)
            {
                m_noteStem.stemPositionX = rightNotesPositionX - stemStokeWidth / 2.0f;
            }
            else
                m_noteStem.stemPositionX = notePositionX + stemStokeWidth / 2.0f;

            m_noteStem.stemStartY = notePositionY + topNotePositionY;
            m_noteStem.stemEndY = notePositionY + stemLength;
        }
    }

    rootNote->noteStem = m_noteStem;

    for (auto articulation : rootNote->articulations)
    {
        if (articulation != nullptr)
            articulation->CalculatePositionAsPaged(displayConstants, rootNote->position.y, rootNote->type == NoteType::Tab, m_noteStem, topNote->position.y - rootNote->position.y, 0.0f);
    }
}

void NoteChord::InitSound(std::shared_ptr<Note> previousNote)
{
    for (auto note : m_notes)
    {
        note->InitSound(previousNote);
    }
}

void NoteChord::OnPlay(std::shared_ptr<Player> player, Transpose transpose, int channel, float velocity)
{
    for (auto note : m_notes)
    {
        note->OnPlay(player, transpose, channel, velocity);
    }
}

void NoteChord::OnStop(std::shared_ptr<Player> player, Transpose transpose, int channel, float velocity)
{
    for (auto note : m_notes)
    {
        note->OnStop(player, transpose, channel, velocity);
    }
}

void NoteChord::OnUpdate(std::shared_ptr<Player> player, Transpose transpose, int channel, float velocity, float beatPositionRelativeToNote, float previousBeatPositionRelativeToNote)
{
    for (auto note : m_notes)
    {
        note->OnUpdate(player, transpose, channel, velocity, beatPositionRelativeToNote, previousBeatPositionRelativeToNote);
    }
}

void NoteChord::OnTranspose(const TranspositionRequest& transposeRequest, const MusicalKey& currentKey)
{
    for (int i = 1; i < m_notes.size(); i++) // skip first note
    {
        m_notes[i]->OnTranspose(transposeRequest, currentKey);
    }
}