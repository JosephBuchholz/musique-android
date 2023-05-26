#include "Arpeggio.h"

void Arpeggio::Render(RenderData& renderData, float notePositionX, float measurePositionY, Vec2<float> offset) const
{
    LOGW("Rendering arpeggio: notePosX: %f, measurePosY: %f, posStart: %s, posEnd: %s", notePositionX, measurePositionY, positionStart.GetPrintableString().c_str(), positionEnd.GetPrintableString().c_str());
    if (type == ArpeggioType::Arpeggio || type == ArpeggioType::ArpeggioUpArrow || type == ArpeggioType::ArpeggioDownArrow)
    {
        if (notes.empty())
            return;

        complexLine->positionStart = positionStart + Vec2<float>{ notes[0]->position.x + notePositionX, measurePositionY + notes[0]->position.y } + offset;
        complexLine->positionEnd = positionEnd + Vec2<float>{ notes[0]->position.x + notePositionX, measurePositionY + notes[notes.size()-1]->position.y } + offset;

        LOGD("complexPosStart: %s, complexPosEnd: %s", complexLine->positionStart.GetPrintableString().c_str(), complexLine->positionEnd.GetPrintableString().c_str());

        complexLine->Render(renderData, { 0.0f, 0.0f });
    }
}

void Arpeggio::RenderDebug(RenderData& renderData) const
{
#if DEBUG_BOUNDING_BOXES
    debugBoundingBox.Render(renderData, 0xFF00FF00);
#endif

    boundingBox.Render(renderData, 0xFF00FFFF);
}

void Arpeggio::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd)
{
    positionStart.x = defaultPositionStart.x;
    positionEnd.x = defaultPositionStart.x;
    //positionEnd.y += 30.0f;

    if (complexLine == nullptr)
    {
        if (type == ArpeggioType::Arpeggio)
        {
            complexLine = std::make_shared<ComplexLine>(ComplexLine::ComplexLineType::ArpeggioUp);
        }
        else if (type == ArpeggioType::ArpeggioUpArrow)
        {
            complexLine = std::make_shared<ComplexLine>(ComplexLine::ComplexLineType::ArpeggioUp);
            complexLine->endGlyph = SMuFLID::wiggleArpeggiatoUpArrow;
        }
        else if (type == ArpeggioType::ArpeggioDownArrow)
        {
            complexLine = std::make_shared<ComplexLine>(ComplexLine::ComplexLineType::ArpeggioDown);
            complexLine->startGlyph = SMuFLID::wiggleArpeggiatoDownArrow;
        }
    }
}