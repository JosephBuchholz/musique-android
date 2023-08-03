#include "InstrumentBracket.h"

void InstrumentBracket::Render(RenderData& renderData, Vec2<float> parentPosition, float height, Vec2<float> offset) const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);

    if (type == InstrumentBracketType::Brace)
    {
        paint.glyphSizeFactor = height / 40.0f;

        renderData.AddGlyph(SMuFLGlyph(SMuFLID::brace, { positionX + parentPosition.x + offset.x, parentPosition.y + offset.y + height }, paint));
    }
    else if (type == InstrumentBracketType::Bracket)
    {
        LOGW("Instrument bracket type 'Bracket' is not supported yet");
    }
    else
    {
        LOGE("Instrument bracket type is 'None'");
    }
}

void InstrumentBracket::CalculateAsPaged(const MusicDisplayConstants& displayConstants, float defaultPositionX)
{
    positionX = defaultPositionX;
}