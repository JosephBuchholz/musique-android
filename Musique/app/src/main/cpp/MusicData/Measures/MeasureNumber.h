/**
 * This files contains the definition of the 'MeasureNumber' class.
 */

#ifndef MUSIQUE_MEASURENUMBER_H
#define MUSIQUE_MEASURENUMBER_H

#include "../BaseElements/NumberElement.h"
#include "../../Collisions/Vec2.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/RenderData.h"

/**
 * This class represents the measure number normally displayed at the start of each system.
 */
class MeasureNumber : public NumberElement {
    friend class Measure;

public:

    MeasureNumber(int number = 0);

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float>& parentPosition);

    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    BoundingBox GetBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition);

private:

    // -- Positioning Attributes --

    // relative to measure
    Vec2<float> position = { 0.0f, 0.0f };
};

#endif //MUSIQUE_MEASURENUMBER_H
