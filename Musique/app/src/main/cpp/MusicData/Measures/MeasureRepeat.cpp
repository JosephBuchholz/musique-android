#include "MeasureRepeat.h"

#include "../../RenderMeasurement.h"

void MeasureRepeat::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);

    float offsetWidth = RenderMeasurement::MeasureGlyph(GetSMuFLID()) / 2.0f;

    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), { position.x + measurePosition.x + offset.x - offsetWidth, position.y + measurePosition.y + offset.y }, paint));
}

SMuFLID MeasureRepeat::GetSMuFLID() const
{
    switch (measureRepeatSlashes)
    {
        case 1: return SMuFLID::repeat1Bar;
        case 2: return SMuFLID::repeat2Bars;
        case 4: return SMuFLID::repeat4Bars;
        default: return SMuFLID::ErrorGlyph;
    }
}

void MeasureRepeat::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, System::SystemMeasureData measureData, Vec2<float> measureDimensions)
{
    //LOGE("measureBeginningWidth: %f | measureWidth: %f", measureData.measureBeginningWidth, measureData.measureWidth);
    position.x = ((measureData.measureWidth - measureData.measureBeginningWidth) / 2.0f) + measureData.measureBeginningWidth;
    position.y = measureDimensions.y / 2.0f;
}