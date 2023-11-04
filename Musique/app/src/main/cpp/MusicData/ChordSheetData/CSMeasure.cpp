#include "CSMeasure.h"

void CSMeasure::Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const
{
    Vec2<float> currentPosition = parentPosition + position;

    renderData.AddLine(Line(currentPosition, { currentPosition.x, currentPosition.y + 40.0f }, Paint()));
    renderData.AddLine(Line({ currentPosition.x + width, currentPosition.y }, { currentPosition.x + width, currentPosition.y + 40.0f }, Paint()));

    LOGV_TAG("CSMeasure", "Rendering Measure!");
    for (const auto& chord : chords)
    {
        chord.Render(renderData, settings, parentPosition + position);
    }

    for (const auto& lyric : lyrics)
    {
        LOGW("Rendering lyric: %s, beatPos: %f", lyric->lyricText.text.c_str(), lyric->beatPosition);
        lyric->Render(renderData, settings, currentPosition);
    }
}

void CSMeasure::Init()
{
    Vec2<float> prevPos = { 0.0f, 0.0f };
    for (const auto& lyric : lyrics)
    {
        Vec2<float> dimensions = lyric->lyricText.GetDimensions(Paint());

        float chordPositionX = GetPositionXFromBeatPositionOfChords(lyric->beatPosition);

        Vec2<float> lyricPosition = { prevPos.x, 55.0f };

        if (lyricPosition.x > chordPositionX)
        {
            // change chord position x
            CSChord& chord = GetChordFromBeatPosition(lyric->beatPosition);

            chord.position.x = lyricPosition.x;
        }
        else if (lyricPosition.x < chordPositionX)
        {
            lyricPosition.x = chordPositionX;
        }

        LOGE("dimensions: %s, lyricPos: %s", dimensions.GetPrintableString().c_str(), lyricPosition.GetPrintableString().c_str());

        lyric->Init(lyricPosition);

        prevPos = { lyricPosition.x + dimensions.x, 0.0f };
    }
}

float CSMeasure::GetPositionXFromBeatPositionOfChords(float beatPosition) const
{
    for (const auto& chord : chords)
    {
        if (beatPosition >= chord.beatPosition) // found
        {
            return chord.position.x;
        }

        // else beatPos < chord.beatPos: continue
    }

    return 0.0f;
}

CSChord& CSMeasure::GetChordFromBeatPosition(float beatPosition)
{
    for (auto& chord : chords)
    {
        if (beatPosition >= chord.beatPosition) // found
        {
            return chord;
        }

        // else beatPos < chord.beatPos: continue
    }
}
