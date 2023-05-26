#ifndef MUSIQUE_MEASUREREPEAT_H
#define MUSIQUE_MEASUREREPEAT_H

#include "../BaseElements/VisibleElement.h"

/**
 * This class represents a measure repeat (also know as a smilie)
 */
class MeasureRepeat : public VisibleElement
{
    friend class Measure;

public:

    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    SMuFLID GetSMuFLID() const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, System::SystemMeasureData measureData, Vec2<float> measureDimensions);

public:

    int measureRepeatLength = 1; // the number of measures in a measure repeat
    int measureRepeatSlashes = 1; // the number of slashes in the simile symbol

    // -- Positioning Attributes --

    // relative to the parent measure
    Vec2<float> position = { 0.0f, 0.0f };
};

#endif //MUSIQUE_MEASUREREPEAT_H
