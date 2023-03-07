#ifndef MUSIQUE_TIMESIGNATURE_H
#define MUSIQUE_TIMESIGNATURE_H

#include "VisibleElement.h"
#include "../RenderData/RenderData.h"

class TimeSignature : public VisibleElement {

public:
    void Render(RenderData& renderData, bool showTimeSignature, float positionX, float measurePositionY, float lineSpacing, int lines, float offsetX = 0.0f, float offsetY = 0.0f) const;

public:
    enum class TimeSignatureDisplayType {
        None = 0, Standard, Common, Cut
    };

    static TimeSignatureDisplayType GetDisplayTypeFromString(const std::string& string) {
        if (string == "common") {
            return TimeSignatureDisplayType::Common;
        } else if (string == "cut") {
            return TimeSignatureDisplayType::Cut;
        }
        return TimeSignatureDisplayType::Standard;
    }

    TimeSignatureDisplayType displayType = TimeSignatureDisplayType::Standard;

    int notes = 4;
    int noteType = 4;
};

#endif // MUSIQUE_TIMESIGNATURE_H