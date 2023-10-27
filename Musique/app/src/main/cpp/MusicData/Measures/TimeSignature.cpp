#include "TimeSignature.h"

void TimeSignature::Render(RenderData& renderData, bool showTimeSignature, float positionX, float measurePositionY, float lineSpacing, int lines, float offsetX, float offsetY) const
{
    // time signature
    if (printObject && showTimeSignature)
    {
        int spaces = lines - 1;
        float staffMiddle = ((float)spaces/2.0f) * lineSpacing;
        float timeDigitHeight = 10.0f;


        switch (displayType)
        {
            case TimeSignatureDisplayType::Common:
            {
                renderData.AddGlyph(SMuFLGlyph(SMuFLID::timeSigCommon, positionX + offsetX, staffMiddle + measurePositionY + offsetY, Paint(color.color)));
                break;
            }
            case TimeSignatureDisplayType::Cut:
            {
                renderData.AddGlyph(SMuFLGlyph(SMuFLID::timeSigCutCommon, positionX + offsetX, staffMiddle + measurePositionY + offsetY, Paint(color.color)));
                break;
            }
            case TimeSignatureDisplayType::Standard:
            {
                float positionY = (staffMiddle - timeDigitHeight) + measurePositionY + offsetY;
                renderData.AddGlyph(SMuFLGlyph(GetTimeSignatureSMuFLID(notes), positionX + offsetX, positionY,Paint(color.color)));
                positionY += 20.0f;
                renderData.AddGlyph(SMuFLGlyph(GetTimeSignatureSMuFLID(noteType), positionX + offsetX, positionY,Paint(color.color)));
                break;
            }
            default:
            {
                LOGW("Time signature display type is not handled");
                break;
            }
        }
    }
}

void TimeSignature::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{

}

SMuFLID TimeSignature::GetTimeSignatureSMuFLID(int time)
{
    switch (time)
    {
        case 0: return SMuFLID::timeSig0;
        case 1: return SMuFLID::timeSig1;
        case 2: return SMuFLID::timeSig2;
        case 3: return SMuFLID::timeSig3;
        case 4: return SMuFLID::timeSig4;
        case 5: return SMuFLID::timeSig5;
        case 6: return SMuFLID::timeSig6;
        case 7: return SMuFLID::timeSig7;
        case 8: return SMuFLID::timeSig8;
        case 9: return SMuFLID::timeSig9;
    }

    return SMuFLID::None;
}
