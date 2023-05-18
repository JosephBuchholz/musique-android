#include "Dynamic.h"

#include "../../RenderMeasurement.h"

void Dynamic::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    Paint paint;
    TextualElement::ModifyPaint(paint);

    BoundingBox bb = RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetDynamicSMuFLID(), 0.0f, 0.0f, paint));

    /*BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize;
    bb.size.x = 25.0f;
    bb.size.y = 25.0f;*/

    boundingBox.position.x = bb.position.x + position.x + parentPosition.x;
    boundingBox.position.y = bb.position.y + position.y + parentPosition.y;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;

    boundingBox.AddPadding(5.0f);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}

void Dynamic::Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset) const
{
    Paint paint;
    TextualElement::ModifyPaint(paint);
    renderData.AddGlyph(SMuFLGlyph(GetDynamicSMuFLID(), position.x + measurePosition.x + offset.x, position.y + measurePosition.y + offset.y, paint));
}

SMuFLID Dynamic::GetDynamicSMuFLID() const
{
    switch (type)
    {
        case DynamicType::Piano: return SMuFLID::dynamicPiano;
        case DynamicType::Pianissimo: return SMuFLID::dynamicPP;
        case DynamicType::OtherPiano:
        {
            switch (dynamicIntensity)
            {
                case 2: return SMuFLID::dynamicPP;
                case 3: return SMuFLID::dynamicPPP;
                case 4: return SMuFLID::dynamicPPPP;
                case 5: return SMuFLID::dynamicPPPPP;
                case 6: return SMuFLID::dynamicPPPPPP;
                default: return SMuFLID::dynamicPiano;
            }
        }
        case DynamicType::Forte: return SMuFLID::dynamicForte;
        case DynamicType::Fortissimo: return SMuFLID::dynamicFF;
        case DynamicType::OtherForte:
        {
            switch (dynamicIntensity)
            {
                case 2: return SMuFLID::dynamicFF;
                case 3: return SMuFLID::dynamicFFF;
                case 4: return SMuFLID::dynamicFFFF;
                case 5: return SMuFLID::dynamicFFFFF;
                case 6: return SMuFLID::dynamicFFFFFF;
                default: return SMuFLID::dynamicForte;
            }
        }
        case DynamicType::MezzoPiano: return SMuFLID::dynamicMP;
        case DynamicType::MezzoForte: return SMuFLID::dynamicMF;
        case DynamicType::SF: return SMuFLID::dynamicSforzando1;
        case DynamicType::SFP: return SMuFLID::dynamicSforzandoPiano;
        case DynamicType::SFPP: return SMuFLID::dynamicSforzandoPianissimo;
        case DynamicType::FP: return SMuFLID::dynamicFortePiano;
        case DynamicType::RF: return SMuFLID::dynamicRinforzando1;
        case DynamicType::RFZ: return SMuFLID::dynamicRinforzando2;
        case DynamicType::SFZ: return SMuFLID::dynamicSforzato;
        case DynamicType::SFFZ: return SMuFLID::dynamicSforzatoFF;
        case DynamicType::FZ: return SMuFLID::dynamicForzando;
        case DynamicType::N: return SMuFLID::dynamicNiente;
        case DynamicType::PF: return SMuFLID::dynamicPF;
        case DynamicType::SFZP: return SMuFLID::dynamicSforzatoPiano;
        default: return SMuFLID::None;
    }
}

SMuFLID Dynamic::GetDynamicSMuFLID(DynamicType type)
{
    switch (type)
    {
        case DynamicType::Piano: return SMuFLID::dynamicPiano;
        case DynamicType::Pianissimo: return SMuFLID::dynamicPP;
        case DynamicType::Forte: return SMuFLID::dynamicForte;
        case DynamicType::Fortissimo: return SMuFLID::dynamicFF;
        case DynamicType::MezzoPiano: return SMuFLID::dynamicMP;
        case DynamicType::MezzoForte: return SMuFLID::dynamicMF;
        default: return SMuFLID::None;
    }
}

void Dynamic::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPos)
{
    position = defaultPos;

    if (defaultPosition.y != INVALID_FLOAT_VALUE)
        position.y = defaultPosition.y;

    if (relativePosition.y != INVALID_FLOAT_VALUE)
        position.y += relativePosition.y;
}