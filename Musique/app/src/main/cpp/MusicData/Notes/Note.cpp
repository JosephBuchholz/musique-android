#include "Note.h"

#define NOTE_DEFAULT_MUTED_NOTE_SOUND PlayableUnpitchedNote::NoteSound::SnareDrum2
#define NOTE_DEFAULT_GRACE_DURATION 0.0625f

void Note::Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float notePositionRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float ls) const
{
    if (tie)
    {
        if (tie->notes.second.get() == this && tabDisplayType == TablatureDisplayType::NoRhythm && type == NoteType::Tab)
            return;
    }

    Vec2<float> noteRenderPosition = position + measurePosition;

    if (isRest) // is a rest
    {
        if (tabDisplayType == TablatureDisplayType::Full)
        {
            RenderRest(renderData, measurePosition.x, lines, ls, 0.0f, measurePosition.y);
        }
    }
    else if (type == NoteType::Tab) // is a tab note
    {
        noteHead.Render(renderData, noteRenderPosition, true);

        // rhythm notation
        if (tabDisplayType == TablatureDisplayType::Full)
        {
            if (!isChord)
            {
                noteStem.Render(renderData, noteRenderPosition, tremoloSingle, isGraceNote, hasSlash, noteHead.GetNoteHeadWidth(renderData.displayConstants));
            }

            noteFlag.Render(renderData, { noteRenderPosition.x + noteStem.stemPositionX, noteRenderPosition.y + noteStem.stemEndY });

            // aug dot
            RenderAugmentationDots(renderData, noteRenderPosition.x, noteRenderPosition.y);
        }
    }
    else // is a standard note
    {
        noteHead.Render(renderData, noteRenderPosition);

        //renderData.AddGlyph(SMuFLGlyph(GetNoteHeadSMuFLID(durationType),renderPositionX + mainPosition.x, renderPositionY + mainPosition.y, Paint(color)));

        // aug dot
        RenderAugmentationDots(renderData, noteRenderPosition.x, noteRenderPosition.y);

        // ledger lines
        if (!isChord)
        {
            Paint ledgerLinePaint;
            ledgerLinePaint.strokeWidth = renderData.displayConstants.legerLineWidth;
            ledgerLinePaint.strokeCap = Paint::Cap::Butt;

            float ledgerLineMargin = renderData.displayConstants.ledgerLineMargin * sizeFactor;
            float noteHeadWidth = noteHead.GetNoteHeadWidth(renderData.displayConstants);
            if (notePositionRelativeToMeasure >= (float)lines) // ledger lines below staff
            {
                int ledgerLineCount = (int)notePositionRelativeToMeasure - lines + 1;
                float y = measurePosition.y + ((float)lines * ls);
                for (int i = 0; i < ledgerLineCount; i++)
                {
                    renderData.AddLine(Line({ noteRenderPosition.x - ledgerLineMargin, y }, { noteRenderPosition.x + noteHeadWidth + ledgerLineMargin, y }, ledgerLinePaint));
                    y += 1.0f * ls;
                }
            }
            if (notePositionRelativeToMeasure < 0.0f) // ledger lines above parentStaff
            {
                int ledgerLineCount = abs((int)notePositionRelativeToMeasure);
                float y = measurePosition.y - (1.0f * ls);
                for (int i = 0; i < ledgerLineCount; i++)
                {
                    renderData.AddLine(Line({ noteRenderPosition.x - ledgerLineMargin, y }, { noteRenderPosition.x + noteHeadWidth + ledgerLineMargin, y }, ledgerLinePaint));
                    y -= 1.0f * ls;
                }
            }
        }

        if (!isChord)
        {
            noteStem.Render(renderData, noteRenderPosition, tremoloSingle, isGraceNote, hasSlash, noteHead.GetNoteHeadWidth(renderData.displayConstants));
        }

        noteFlag.Render(renderData, { noteRenderPosition.x + noteStem.stemPositionX, noteRenderPosition.y + noteStem.stemEndY });

        if (accidental)
            accidental->Render(renderData, noteRenderPosition);
    }

    // render articulations
    for (const auto& articulation : articulations)
    {
        if (articulation != nullptr)
            articulation->Render(renderData, { noteRenderPosition.x + noteHead.GetCenterPositionX(renderData.displayConstants), noteRenderPosition.y });
    }

    // render techniques
    for (const auto& technique : techniques)
    {
        if (technique != nullptr)
            technique->Render(renderData, { noteRenderPosition.x + noteHead.GetCenterPositionX(renderData.displayConstants), noteRenderPosition.y });
    }

    // render ornament
    for (const auto& ornament : ornaments)
    {
        if (ornament != nullptr)
            ornament->Render(renderData, noteRenderPosition.x + noteHead.GetCenterPositionX(renderData.displayConstants), measurePosition.y);
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

    for (const auto& glissSlide : glissSlides)
    {
        if (glissSlide->notes.first.get() == this && glissSlide->notes.second != nullptr) // this note is the start of glissSlide
        {
            // TODO: fix: currently assuming that the gliss is only contained in a single measure

            Vec2<float> otherNoteMeasurePosition = measurePosition;

            if (glissSlide->notes.first->measureIndex != glissSlide->notes.second->measureIndex)
            {
                otherNoteMeasurePosition.x = nextMeasurePositionX;
            }

            glissSlide->Render(renderData, {position.x + measurePosition.x + (noteHead.GetCenterPositionX(renderData.displayConstants)), position.y + measurePosition.y },
                               { glissSlide->notes.second->position.x + otherNoteMeasurePosition.x + (glissSlide->notes.second->noteHead.GetCenterPositionX(renderData.displayConstants)), glissSlide->notes.second->position.y + otherNoteMeasurePosition.y }, true);
        }
        else if (glissSlide->notes.second.get() == this && glissSlide->notes.first != nullptr) // if this note is the end of the glissSlide
        {
            if (glissSlide->isBroken)
            {
                // render the "second" (broken) glissSlide
                Vec2<float> startNotePosition = measurePosition + GetCenterPosition(renderData.displayConstants);

                glissSlide->Render(renderData, startNotePosition, { 0.0f, 0.0f }, false);
            }
        }
    }

    if (tie && !(tabDisplayType == TablatureDisplayType::NoRhythm && type == NoteType::Tab))
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

            tie->Render(renderData, startNotePosition, endNotePosition, true);
        }
        else if (tie->notes.second.get() == this && tie->notes.first != nullptr) // if this note is the end of the tie
        {
            if (tie->isBroken)
            {
                // render the "second" (broken) tie
                Vec2<float> startNotePosition = measurePosition + GetCenterPosition(renderData.displayConstants);

                tie->Render(renderData, startNotePosition, { 0.0f, 0.0f }, false);
            }
        }
    }

    for (const auto& lyric: lyrics)
    {
        lyric.Render(renderData, position.x + measurePosition.x, measurePosition.y);
    } // lyrics loop
}

void Note::RenderDebug(RenderData& renderData, TablatureDisplayType tabDisplayType, float notePositionRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float ls) const
{
    Vec2<float> noteRenderPosition = position + measurePosition;

    // render articulations
    for (const auto& articulation : articulations)
    {
        if (articulation != nullptr)
            articulation->RenderDebug(renderData, { noteRenderPosition.x + noteHead.GetCenterPositionX(renderData.displayConstants), noteRenderPosition.y });
    }

    for (const auto& lyric : lyrics)
    {
        lyric.RenderDebug(renderData, position.x + measurePosition.x, measurePosition.y);
    }
}

void Note::RenderDebug(RenderData& renderData) const
{
    boundingBox.Render(renderData, (int)0x660000FF);

    if (fermata)
        fermata->RenderDebug(renderData);

    for (const auto& glissSlide : glissSlides)
    {
        if (glissSlide->notes.first.get() == this)
            glissSlide->RenderDebug(renderData);
    }

    for (const auto& ornament : ornaments)
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
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(durationType), { positionX, positionY }, Paint(color)));
    }
    else if (durationType == NoteValue::Whole) {
        positionY -= ls;
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(durationType), { positionX, positionY }, Paint(color)));
    }
    else {
        renderData.AddGlyph(SMuFLGlyph(GetRestSMuFLID(durationType), { positionX, positionY }, Paint(color)));
    }
}

void Note::RenderAugmentationDots(RenderData& renderData, float notePositionX, float notePositionY) const
{
    // aug dot
    for (const auto& dot : dots)
    {
        dot.Render(renderData, { notePositionX, notePositionY });
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

void Note::InitSound(const std::shared_ptr<Note>& previousNote)
{
    soundBeatPosition = beatPosition;
    soundDuration = duration.duration - 0.05f;

    if (isGraceNote)
    {
        soundBeatPosition = beatPosition;
        soundDuration = NOTE_DEFAULT_GRACE_DURATION;
    }

    if (previousNote)
    {
        if (previousNote->isGraceNote)
        {
            soundBeatPosition = previousNote->soundBeatPosition + previousNote->soundDuration;
        }
    }

    for (const auto& articulation : articulations)
    {
        articulation->ModifySoundDuration(soundDuration);
    }
}

void Note::OnPlay(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity)
{
    isPlaying = true;

    for (const auto& articulation : articulations)
    {
        articulation->ModifyVelocity(velocity);
    }

    if (noteHead.type == NoteHead::NoteHeadType::X)
    {
        PlayableUnpitchedNote note;
        note.sound = NOTE_DEFAULT_MUTED_NOTE_SOUND;
        player->PlayUnpitchedNote(note);
    }
    else if (!isRest)
    {
        if (tie)
        {
            if (tie->notes.second.get() != this) // is the first note in tie
            {
                PlayPitch(player, transpose, channel, velocity);
            }
        }
        else
        {
            PlayPitch(player, transpose, channel, velocity);
        }
    }
}

void Note::OnStop(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity)
{
    isPlaying = false;

    if (noteHead.type == NoteHead::NoteHeadType::X)
    {
        PlayableUnpitchedNote note;
        note.sound = NOTE_DEFAULT_MUTED_NOTE_SOUND;
        player->StopUnpitchedNote(note);
    }
    else if (!isRest)
    {
        if (tie)
        {
            if (tie->notes.first.get() != this) // is the second note in tie
            {
                StopPitch(player, transpose, channel, velocity);
            }
        }
        else
        {
            StopPitch(player, transpose, channel, velocity);
        }
    }
}

void Note::PlayPitch(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity)
{
    PlayableNote note;
    note.pitch = pitch;
    note.pitch.octave += transpose.octaveChange;
    note.pitch.SetPitchValue(note.pitch.GetPitchValue() + transpose.chromatic);
    note.velocity = velocity;
    player->PlayNote(note, channel);
}

void Note::StopPitch(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity)
{
    PlayableNote note;
    note.pitch = pitch;
    note.pitch.octave += transpose.octaveChange;
    note.pitch.SetPitchValue(note.pitch.GetPitchValue() + transpose.chromatic);
    note.velocity = velocity;
    player->StopNote(note, channel);

    for (const auto& glissSlide : glissSlides)
    {
        if (glissSlide)
        {
            if (glissSlide->notes.first.get() == this)
            {
                int notes = std::abs(glissSlide->notes.second->pitch.GetPitchValue() - pitch.GetPitchValue());

                bool isSlideUp = true;
                if (glissSlide->notes.second->pitch < pitch)
                    isSlideUp = false;

                PlayableNote previousNote;
                previousNote.pitch = pitch;
                if (isSlideUp)
                    previousNote.pitch.SetPitchValue(previousNote.pitch.GetPitchValue() + (notes - 1));
                else
                    previousNote.pitch.SetPitchValue(previousNote.pitch.GetPitchValue() - (notes - 1));
                previousNote.pitch.octave += transpose.octaveChange;
                previousNote.velocity = velocity;
                player->StopNote(previousNote, channel);
            }
        }
    }
}

void Note::OnUpdate(const std::shared_ptr<Player>& player, Transpose transpose, int channel, float velocity, float beatPositionRelativeToNote, float previousBeatPositionRelativeToNote)
{
    for (const auto& technique : techniques)
    {
        technique->OnUpdate(player, beatPositionRelativeToNote, soundDuration, channel);
    }

    for (const auto& glissSlide : glissSlides)
    {
        if (glissSlide)
        {
            if (glissSlide->notes.first.get() == this)
            {
                int notes = std::abs(glissSlide->notes.second->pitch.GetPitchValue() - pitch.GetPitchValue());

                bool isSlideUp = true;
                if (glissSlide->notes.second->pitch < pitch)
                    isSlideUp = false;

                float timeInterval = soundDuration / notes;

                for (int i = 1; i < notes; i++)
                {
                    if (beatPositionRelativeToNote >= timeInterval * i && previousBeatPositionRelativeToNote <= timeInterval * i)
                    {
                        PlayableNote previousNote;
                        previousNote.pitch = pitch;
                        if (isSlideUp)
                            previousNote.pitch.SetPitchValue(previousNote.pitch.GetPitchValue() + (i - 1));
                        else
                            previousNote.pitch.SetPitchValue(previousNote.pitch.GetPitchValue() - (i - 1));
                        previousNote.pitch.octave += transpose.octaveChange;
                        previousNote.velocity = velocity;
                        player->StopNote(previousNote, channel);

                        PlayableNote note;
                        note.pitch = pitch;
                        if (isSlideUp)
                            note.pitch.SetPitchValue(note.pitch.GetPitchValue() + i);
                        else
                            note.pitch.SetPitchValue(note.pitch.GetPitchValue() - i);
                        note.pitch.octave += transpose.octaveChange;
                        note.velocity = velocity;
                        player->PlayNote(note, channel);

                        //LOGW("previousNote.pitch: %s ||| note.pitch: %s ||| this->pitch: %s", previousNote.pitch.GetPrintableString().c_str(), note.pitch.GetPrintableString().c_str(), pitch.GetPrintableString().c_str());
                    }
                }
            }
        }
    }
}

float Note::GetMinWidth() const
{
    float width = 0.0f;
    width += duration.duration * 5.0f * 10.0f; // should do a bit more calculations here
    if (!lyrics.empty())
        width += 20.0f;
    return width;
}

void Note::CalculateDurationTypeFromString(const std::string& s)
{
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
    // update note head
    noteHead.noteDuration = durationType;
    noteHead.noteType = type;
    noteHead.fret = fret;
    noteHead.CalculateAsPaged(displayConstants, noteSize);

    // update note flag
    if ((durationType == NoteValue::Eighth || durationType == NoteValue::Sixteenth || durationType == NoteValue::ThirtySecond) && !isChord && beamData.empty())
    {
        float defaultSize = 1.0f;
        if (noteSize == NoteSize::Grace)
            defaultSize = displayConstants.graceNoteSize;
        else if (noteSize == NoteSize::Cue)
            defaultSize = displayConstants.cueNoteSize;
        noteFlag.noteDuration = durationType;
        if (noteStem.stemType == NoteStem::StemType::Up) {
            noteFlag.type = NoteFlag::Type::Up;
        } else if (noteStem.stemType == NoteStem::StemType::Down) {
            noteFlag.type = NoteFlag::Type::Down;
        }
        noteFlag.CalculateAsPaged(displayConstants, defaultSize);
    }
    else
    {
        noteFlag.type = NoteFlag::Type::None;
    }

    if (accidental)
        accidental->CalculateAsPaged(displayConstants, noteSize);

    if (noteSize == NoteSize::Grace)
        sizeFactor = displayConstants.graceNoteSize;
    else if (noteSize == NoteSize::Cue)
        sizeFactor = displayConstants.cueNoteSize;

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

            stemLength *= sizeFactor;

            noteStem.stemPositionX = noteHead.GetCenterPositionX(displayConstants);

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

        float stemStokeWidth = displayConstants.stemLineWidth * sizeFactor;

        float stemLength = 30.0f * sizeFactor;
        if (noteStem.stemType == NoteStem::StemType::Up)
        {
            noteStem.stemPositionX = notePositionX + noteWidth - stemStokeWidth;

            noteStem.stemStartY = notePositionY;
            noteStem.stemEndY = notePositionY - stemLength;
        }
        else if (noteStem.stemType == NoteStem::StemType::Down)
        {
            noteStem.stemPositionX = notePositionX + stemStokeWidth;

            noteStem.stemStartY = notePositionY;
            noteStem.stemEndY = notePositionY + stemLength;
        }
    }

    for (const auto& articulation : articulations)
    {
        if (articulation != nullptr)
            articulation->CalculatePositionAsPaged(displayConstants, position.y, type == NoteType::Tab, noteStem, 0.0f, 0.0f);
    }

    for (const auto& technique : techniques)
    {
        if (technique != nullptr)
            technique->CalculatePositionAsPaged(displayConstants, position.y, type == NoteType::Tab, noteStem, 0.0f, 0.0f);
    }

    for (const auto& ornament : ornaments)
    {
        if (ornament != nullptr)
            ornament->CalculatePositionAsPaged(displayConstants, { 0.0f, 0.0f });
    }

    for (auto& dot : dots)
    {
        dot.CalculatePositionAsPaged(displayConstants, ((position.y / displayConstants.lineSpacing) - floor(position.y / displayConstants.lineSpacing)) == 0.0f, type == NoteType::Tab, noteHead.GetNoteHeadWidth(displayConstants));
    }

    if (fermata)
        fermata->CalculatePositionAsPaged(displayConstants, { 0.0f, -20.0f });

    if (tremoloSingle)
        tremoloSingle->CalculatePositionAsPaged(displayConstants);

    for (const auto& glissSlide : glissSlides)
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
        AboveBelowType defaultPlacement = AboveBelowType::Above;

        if (noteStem.stemType == NoteStem::StemType::Up || voice != 1)
        {
            defaultPlacement = AboveBelowType::Below;
        }

        tie->CalculatePositionAsPaged(displayConstants, { 0.0f, 0.0f }, { 0.0f, 0.0f }, defaultPlacement, isChord);
    }
}

Vec2<float> Note::GetCenterPosition(const MusicDisplayConstants& displayConstants) const
{
    return { position.x + noteHead.GetCenterPositionX(displayConstants), position.y };
}

BoundingBox Note::GetBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const
{
    BoundingBox bb;

    bb = BoundingBox::CombineBoundingBoxes(noteHead.GetBoundingBoxRelativeToParent(displayConstants),
                                           noteStem.GetBoundingBoxRelativeToParent(displayConstants));

    bb.position += position;

    return bb;
}

BoundingBox Note::GetTotalBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const
{
    BoundingBox bb;

    bb = BoundingBox::CombineBoundingBoxes(noteHead.GetBoundingBoxRelativeToParent(displayConstants),
                                           noteStem.GetBoundingBoxRelativeToParent(displayConstants));

    bb.position += position;

    for (const auto& articulation : articulations)
    {
        if (articulation != nullptr)
        {
            BoundingBox articulationBoundingBox = articulation->GetBoundingBox();
            articulationBoundingBox.position += position;
            bb = BoundingBox::CombineBoundingBoxes(bb, articulationBoundingBox);
        }
    }

    for (const auto& lyric : lyrics)
    {
        BoundingBox lyricBoundingBox = lyric.GetBoundingBoxRelativeToParent(position.x, 0.0f);
        //lyricBoundingBox.position += position;
        bb = BoundingBox::CombineBoundingBoxes(bb, lyricBoundingBox);
    }

    if (fermata)
    {
        bb = BoundingBox::CombineBoundingBoxes(bb, fermata->GetBoundingBoxRelativeToParent());
    }

    return bb;
}

void Note::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition)
{
    noteHead.UpdateBoundingBox(displayConstants, position + parentPosition);
    noteStem.UpdateBoundingBox(displayConstants, position + parentPosition);

    boundingBox = BoundingBox::CombineBoundingBoxes(noteHead.boundingBox, noteStem.boundingBox);

    /*if (type == NoteType::Tab)
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
        boundingBox.size.y = 10.0f;*/

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif

    if (fermata)
        fermata->UpdateBoundingBox({ position.x + parentPosition.x, parentPosition.y });

    for (const auto& glissSlide : glissSlides)
    {
        if (glissSlide->notes.first.get() == this)
        {
            glissSlide->UpdateBoundingBox({ 0.0f, 0.0f });
        }
    }

    for (const auto& ornament : ornaments)
    {
        if (ornament != nullptr)
            ornament->UpdateBoundingBox({ position.x + parentPosition.x, parentPosition.y });
    }

    for (Lyric& lyric : lyrics)
    {
        lyric.UpdateBoundingBox(Vec2<float>{ position.x + parentPosition.x, parentPosition.y });
    }
}

void Note::UpdateTieAndGlissSlide(bool measureStartsNewSystem)
{
    if (measureStartsNewSystem)
    {
        if (tie)
        {
            if (tie->notes.second.get() == this) // the second note of tie
            {
                if (tie->notes.first->measureIndex != tie->notes.second->measureIndex) // the notes of the tie are on different systems
                {
                    tie->isBroken = true;
                }
            }
        }

        for (const auto &glissSlide: glissSlides)
        {
            if (glissSlide->notes.second.get() == this) // the second note
            {
                if (glissSlide->notes.first->measureIndex != glissSlide->notes.second->measureIndex) // the notes are on different systems
                {
                    glissSlide->isBroken = true;
                }
            }
        }
    }
}

void Note::OnTranspose(const TranspositionRequest& transposeRequest, const MusicalKey& currentKey)
{
    if (type == NoteType::Tab)
    {
        if (transposeRequest.transposeTablatureType == TranspositionRequest::TransposeTablatureType::NoCapo)
        {
            //TranspositionRequest::TransposeDirection direction = transposeRequest.GetTransposeDirection();

            //if (direction == TranspositionRequest::TransposeDirection::Up)
            //{
            //    fret += GetInter;
            //}
            //else if (direction == TranspositionRequest::TransposeDirection::Down)
            //{
            //    fret -= 1;
            //}
            fret += transposeRequest.GetInterval();
        }
        else
        {
            return;
        }
    }

    pitch.OnTranspose(transposeRequest, currentKey);

    if (accidental)
        accidental->CalculateTypeFromPitch(pitch, transposeRequest.key);
}

SMuFLID Note::GetRestSMuFLID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::None: return SMuFLID::None;
        case NoteValue::Whole: return SMuFLID::restWhole;
        case NoteValue::Half: return SMuFLID::restHalf;
        case NoteValue::Quarter: return SMuFLID::restQuarter;
        case NoteValue::Eighth: return SMuFLID::rest8th;
        case NoteValue::Sixteenth: return SMuFLID::rest16th;
        case NoteValue::ThirtySecond: return SMuFLID::rest32nd;
        default: return SMuFLID::restDoubleWhole;
    }

    return SMuFLID::None;
}
