#include "TimeSignature.h"

void TimeSignature::Render(RenderData& renderData, bool showTimeSignature, float positionX, float measurePositionY, float lineSpacing, int lines, float offsetX, float offsetY) const
{
    // time signature
    if (printObject && showTimeSignature)
    {
        int spaces = lines - 1;
        float staffMiddle = ((float)spaces/2.0f) * lineSpacing;
        float timeDigitHeight = 10.0f;
        float positionY = (staffMiddle - timeDigitHeight) + measurePositionY + offsetY;

        renderData.AddGlyph(SMuFLGlyph(GetTimeSignatureSMuFLID(notes),positionX + offsetX, positionY,Paint(0xff000000)));
        positionY += 20.0f;
        renderData.AddGlyph(SMuFLGlyph(GetTimeSignatureSMuFLID(noteType),positionX + offsetX, positionY,Paint(0xff000000)));
    }

    /*// time signature
    if (printObject && showTimeSignature)
    {
        int spaces = lines - 1;
        float staffMiddle = ((float)spaces/2.0f) * lineSpacing;
        float timeDigitHeight = 10.0f;


        switch (displayType)
        {
            case TimeSignatureDisplayType::Common:
            {
                //renderData.AddGlyph(SMuFLGlyph(SMuFLID::timeSigCommon, positionX + offsetX, staffMiddle + measurePositionY + offsetY, Paint(color.color)));
                break;
            }
            case TimeSignatureDisplayType::Cut:
            {
                //renderData.AddGlyph(SMuFLGlyph(SMuFLID::timeSigCutCommon, positionX + offsetX, staffMiddle + measurePositionY + offsetY, Paint(color.color)));
                break;
            }
            case TimeSignatureDisplayType::Standard:
            {
                float positionY = (staffMiddle - timeDigitHeight) + measurePositionY + offsetY;
                renderData.AddGlyph(SMuFLGlyph(GetTimeSignatureSMuFLID(notes), positionX + offsetX, positionY,Paint(color.color)));
                positionY += 20.0f;
                renderData.AddGlyph(SMuFLGlyph(GetTimeSignatureSMuFLID(noteType), positionX + offsetX, positionY,Paint(0xff000000)));
                break;
            }
            default:
            {
                LOGW("Time signature display type is not handled");
                break;
            }
        }
    }*/
}