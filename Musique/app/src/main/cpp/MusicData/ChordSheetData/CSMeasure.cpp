#include "CSMeasure.h"

void CSMeasure::Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const
{
    Vec2<float> currentPosition = parentPosition + position;

    renderData.AddLine(Line(currentPosition, { currentPosition.x, currentPosition.y + 40.0f }, Paint()));
    renderData.AddLine(Line({ currentPosition.x + width, currentPosition.y }, { currentPosition.x + width, currentPosition.y + 40.0f }, Paint()));

    LOGV_TAG("CSMeasure", "Rendering Measure!");
    for (const auto& chord : chords)
    {
        chord->Render(renderData, settings, parentPosition + position);
    }

    if (isFirstMeasureOfSystem && !lyrics.empty())
    {
        std::shared_ptr<LyricalPhrase> lyricalPhrase = lyrics[0]->parentLyricalPhrase;

        for (const auto& lyric : lyricalPhrase->lyrics)
        {
            if (lyric->isPickupToNextMeasure)
            {
                lyric->Render(renderData, settings, currentPosition);
            }
            else
                break;
        }
    }

    for (const auto& lyric : lyrics)
    {
        LOGI_TAG("CSMeasure", "Rendering lyric: %s, beatPos: %f", lyric->lyricText.text.c_str(), lyric->beatPosition);
        if (!lyric->isPickupToNextMeasure)
            lyric->Render(renderData, settings, currentPosition);
    }
}

void CSMeasure::Init()
{
    Vec2<float> previousPosition = { 10.0f, 0.0f };
    for (const auto& chord : chords)
    {
        Vec2<float> dimensions = chord->GetDimensions();

        Vec2<float> position = { previousPosition.x, 25.0f };
        chord->Init(position);

        if (position.x + dimensions.x + chord->duration * 30.0f > width)
        {
            width = position.x + dimensions.x + chord->duration * 30.0f;
        }

        previousPosition = position;
        previousPosition.x += dimensions.x + chord->duration * 30.0f;
    }


    if (isFirstMeasureOfSystem && !lyrics.empty())
    {
        std::shared_ptr<LyricalPhrase> lyricalPhrase = lyrics[0]->parentLyricalPhrase;

        Vec2<float> prevPos = { 0.0f, 0.0f };
        for (int i = (int)lyricalPhrase->lyrics.size() - 1; i >= 0; i--)
        {
            std::shared_ptr<CSLyric> lyric = lyricalPhrase->lyrics[i];

            if (lyric->isPickupToNextMeasure)
            {
                Vec2<float> dimensions = lyric->lyricText.GetDimensions(Paint());

                Vec2<float> lyricPosition = { prevPos.x - dimensions.x, 55.0f };

                lyric->Init(lyricPosition);

                prevPos = lyricPosition;
            }
        }
    }

    Vec2<float> prevPos = { 0.0f, 0.0f };
    for (const auto& lyric : lyrics)
    {
        if (lyric->isPickupToNextMeasure)
            continue;

        Vec2<float> dimensions = lyric->lyricText.GetDimensions(Paint());

        float chordPositionX = GetPositionXFromBeatPositionOfChords(lyric->beatPosition);

        Vec2<float> lyricPosition = { prevPos.x, 55.0f };

        if (lyricPosition.x > chordPositionX)
        {
            // change chord position x
            std::shared_ptr<CSChord> chord = GetChordFromBeatPosition(lyric->beatPosition);

            if (chord != nullptr)
                chord->position.x = lyricPosition.x;
        }
        else if (lyricPosition.x < chordPositionX)
        {
            lyricPosition.x = chordPositionX;
        }

        if (lyricPosition.x + dimensions.x > width)
        {
            width = lyricPosition.x + dimensions.x;
        }

        LOGE("dimensions: %s, lyricPos: %s", dimensions.GetPrintableString().c_str(), lyricPosition.GetPrintableString().c_str());

        lyric->Init(lyricPosition);

        prevPos = { lyricPosition.x + dimensions.x, 0.0f };
    }

    float chordPosX = 0.0f;
    float lyricPosX = 0.0f;

    if (!chords.empty())
        chordPosX = chords.back()->GetDimensions().x + chords.back()->position.x + chords.back()->duration * 30.0f;

    if (!lyrics.empty())
        lyricPosX = lyrics.back()->lyricText.GetDimensions(Paint()).x + lyrics.back()->position.x;

    width = std::max(chordPosX, lyricPosX);
}

float CSMeasure::GetPositionXFromBeatPositionOfChords(float beatPosition) const
{
    for (const auto& chord : chords)
    {
        if (beatPosition == chord->beatPosition) // found
        {
            return chord->position.x;
        }

        // else beatPos < chord.beatPos: continue
    }

    return 0.0f;
}

std::shared_ptr<CSChord> CSMeasure::GetChordFromBeatPosition(float beatPosition)
{
    float previousChordBeatPosition = 0.0f;
    for (const auto& chord : chords)
    {
        if (beatPosition >= previousChordBeatPosition && beatPosition <= chord->beatPosition) // found
        {
            return chord;
        }

        previousChordBeatPosition = chord->beatPosition;

        // else beatPos < chord.beatPos: continue
    }

    return nullptr;
}

BoundingBox CSMeasure::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants) const
{
    BoundingBox totalBoundingBox;

    totalBoundingBox.size.x = width;
    totalBoundingBox.size.y = 100.0f;

    return totalBoundingBox;
}
