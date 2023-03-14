/**
 * This file contains the definition of the `DynamicWedge` class.
 */

#ifndef MUSIQUE_DYNAMICWEDGE_H
#define MUSIQUE_DYNAMICWEDGE_H

#include "../VisibleElement.h"
#include "../../Vec2.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/RenderData.h"

/**
 * This class represents a cresc. or dim. wedge/hairpin marking.
 */
class DynamicWedge : public VisibleElement {
    friend class Song;
    friend class MusicXMLParser;

public:

    void Render(RenderData& renderData, float measurePositionX, float measurePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const;

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd, float defSpread);

public:

    enum class WedgeType
    {
        None = 0, Crescendo, Diminuendo
    } type = WedgeType::None;

    // starting beat position
    float beatPositionStart = 0.0f; // the position in the measure in beats(quarter notes)
    float beatPositionInSongStart = 0.0f; // the position in the song(not counting repeats) in beats(quarter notes)

    // ending beat position
    float beatPositionEnd = 0.0f;
    float beatPositionInSongEnd = 0.0f;

    // -- Positioning Attributes --

    // relative to measure
    Vec2<float> positionStart = { 0.0f, 0.0f };
    Vec2<float> positionEnd = { 0.0f, 0.0f };

    // the height of the wedge
    float spread = 0.0f;

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> defaultPositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };

    Vec2<float> relativePositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };

    float defaultSpread = INVALID_FLOAT_VALUE;
};

#endif // MUSIQUE_DYNAMICWEDGE_H
