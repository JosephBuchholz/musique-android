/**
 * This file contains the definition for the 'PageNumber' class.
 */

#ifndef MUSIQUE_PAGENUMBER_H
#define MUSIQUE_PAGENUMBER_H

#include "../BaseElements/NumberElement.h"
#include "../../Collisions/Vec2.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/RenderData.h"

/**
 * This class represents a page number.
 */
class PageNumber : public NumberElement
{
    friend class Song;

public:

    PageNumber(int number = 0);

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float>& parentPosition);

    void Render(RenderData& renderData, Vec2<float> pagePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

protected:

    void CalculatePosition(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition);

private:

    // -- Positioning Attributes --

    // relative to measure
    Vec2<float> position = { 0.0f, 0.0f };
};

#endif //MUSIQUE_PAGENUMBER_H
