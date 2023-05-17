/**
 * This file contains the definition for the 'Marker' class.
 */

#ifndef MUSIQUE_MARKER_H
#define MUSIQUE_MARKER_H

#include "../BaseElements/VisibleElement.h"

#include "../../Collisions/Vec2.h"
#include "../Types.h"

#include "../../RenderData/RenderData.h"

/**
 * This class represents a musical markers such as the coda and segno
 */
class Marker : public VisibleElement {
    friend class Song;
    friend class MusicXMLParser;

public:

    /**
     * Renders this class.
     *
     * @param[out] renderData The RenderData object to render to.
     */
    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

    SMuFLID GetSMuFLID() const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(Vec2<float> parentPosition);

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPosition);

public:

    enum class MarkerType
    {
        None = 0, Coda, Segno
    } type = MarkerType::None;

    // -- Positioning Attributes --

    Vec2<float> position = { 0.0f, 0.0f }; // relative to measure

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif //MUSIQUE_MARKER_H
