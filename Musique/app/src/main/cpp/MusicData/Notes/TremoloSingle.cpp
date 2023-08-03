#include "TremoloSingle.h"

void TremoloSingle::Render(RenderData& renderData, Vec2<float> notePosition, Vec2<float> offset) const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), position + notePosition + offset, paint));
}

SMuFLID TremoloSingle::GetSMuFLID() const
{
    switch (type)
    {
        case Type::Normal:
        {
            switch (tremoloMarks)
            {
                case 1: return SMuFLID::tremolo1;
                case 2: return SMuFLID::tremolo2;
                case 3: return SMuFLID::tremolo3;
                case 4: return SMuFLID::tremolo4;
                case 5: return SMuFLID::tremolo5;
                default: return SMuFLID::ErrorGlyph;
            }
        }
        case Type::Unmeasured: return SMuFLID::unmeasuredTremolo;
        case Type::BuzzRoll: return SMuFLID::buzzRoll;
        default: return SMuFLID::ErrorGlyph;
    }
}

void TremoloSingle::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{

}