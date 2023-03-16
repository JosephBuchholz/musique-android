#include "Direction.h"

void Direction::Render(RenderData& renderData, float measurePositionX, float measurePositionY, float offsetX, float offsetY) const
{
    if (!rehearsals.empty())
    {
        Rehearsal rehearsal = rehearsals[0];
        Paint paint = Paint();
        paint.textSize = 16.0f;
        if (rehearsal.fontStyle == FontStyle::Italic)
            paint.isItalic = true;
        if (rehearsal.fontWeight == FontWeight::Bold)
            paint.isBold = true;
        paint.textSize = rehearsal.fontSize.size;
        //float positionX = song->GetPositionXInMeasure(direction.beatPositionInSong, measure->index) + measureXPosition;
        float positionX = rehearsal.positionX + measurePositionX;
        float positionY = rehearsal.positionY + measurePositionY;

        renderData.AddText(Text(rehearsal.text.string, positionX + offsetX, positionY + offsetY, Paint(rehearsal.color.color, paint)));
        //renderData.AddDebugDot(positionX + offsetX, positionY + offsetY);

        LOGE("Rehersal: %s, X: %f, Y: %f", rehearsal.text.string.c_str(), rehearsal.positionX, rehearsal.positionY);
    }
    else if (!words.empty())
    {
        Words wordsObject = words[0];
        Paint paint = Paint();
        paint.textSize = 16.0f;
        if (wordsObject.fontStyle == FontStyle::Italic)
            paint.isItalic = true;
        if (wordsObject.fontWeight == FontWeight::Bold)
            paint.isBold = true;
        paint.textSize = wordsObject.fontSize.size;

        //float positionX = song->GetPositionXInSong(direction.beatPositionInSong, measure->index);
        float positionX = wordsObject.positionX + measurePositionX;
        float positionY = wordsObject.positionY + measurePositionY;

        renderData.AddText(Text(wordsObject.text.string, positionX + offsetX, positionY + offsetY, Paint(wordsObject.color.color, paint)));
        //renderData.AddDebugDot(positionX + offsetX, positionY + offsetY);

        LOGE("words: %s, X: %f, Y: %f", wordsObject.text.string.c_str(), wordsObject.positionX, wordsObject.positionY);
    }
    else if (metronomeMark != nullptr)
    {
        float positionX = metronomeMark->positionX + measurePositionX;
        float positionY = metronomeMark->positionY + measurePositionY;
        //std::string string = direction.metronomeMark->tempo + (char)0xECA6;
        std::string string = " = " + metronomeMark->tempo;
        //std::u16string s = direction.metronomeMark->tempo;
        //LOGE("metronomeMark: %s", string.c_str());

        //std::u16string s;
        //s.append((char16_t)0xECA6);
        std::vector<uint16_t> chars;
        chars.push_back((uint16_t)GetMetronomeNoteSMuFLID(metronomeMark->mainNoteUnit.noteValue));
        if (metronomeMark->mainNoteUnit.isDotted)
            chars.push_back((uint16_t)SMuFLID::metAugmentationDot);

        for (auto& c : string)
        {
            if (c == '\0')
                break;
            chars.push_back(c);
        }

        std::vector<TextSpan> spans;
        Paint glyphPaint = Paint(metronomeMark->color.color);
        glyphPaint.useMusicFont = true;
        glyphPaint.textSize = 40.0f;
        glyphPaint.align = Paint::Align::Left;

        Paint paint = Paint(metronomeMark->color.color);
        paint.isBold = true;
        paint.textSize = 26.0f;
        paint.align = Paint::Align::Left;

        if (metronomeMark->mainNoteUnit.isDotted)
        {
            spans.emplace_back(0, 2, glyphPaint);
            spans.emplace_back(2, 10, paint);
        }
        else
        {
            spans.emplace_back(0, 1, glyphPaint);
            spans.emplace_back(1, 10, paint);
        }

        renderData.AddSpannableText(std::make_shared<SpannableText>(chars, positionX + offsetX, positionY + offsetY, spans, Paint(metronomeMark->color.color)));
        //renderData.AddText(Text(string, positionX + offsetX, positionY + offsetY, Paint(direction.metronomeMark->color.color)));
        //renderData.AddDebugDot(positionX + offsetX, positionY + offsetY);
        LOGE("metronome mark: X: %f, Y: %f", metronomeMark->positionX, metronomeMark->positionY);
    }
    else if (!dynamics.empty())
    {
        for (const Dynamic& dynamic : dynamics)
        {
            dynamic.Render(renderData, measurePositionX, measurePositionY);
        }
    }

    if (dynamicWedge != nullptr)
    {
        dynamicWedge->Render(renderData, measurePositionX, measurePositionY);
    }

    if (bracketDirection != nullptr)
    {
        bracketDirection->Render(renderData, { measurePositionX, measurePositionY });
    }
}