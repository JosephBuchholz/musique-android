/**
 * This file contains the definition of the `BracketDirection` class.
 */

#ifndef MUSIQUE_BRACKETDIRECTION_H
#define MUSIQUE_BRACKETDIRECTION_H

#include "../VisibleElement.h"
#include "../LineElement.h"
#include "../../Collisions/Vec2.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/RenderData.h"

/**
 * This class represents a bracket for part of a direction marking.
 *
 * Note: MusicXML dashes and brackets are represented as this one class.
 */
class BracketDirection : public VisibleElement, public LineElement {
    friend class Song;
    friend class MusicXMLParser;

public:

    enum class LineEndType
    {
        None = 0, Up, Down, Both, Arrow, NoEnd
    };

    BracketDirection() {}

    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd);

private:

    static void RenderLineEnd(RenderData& renderData, Vec2<float> position, LineEndType lineEndType, float endLength, const Paint& paint = Paint(), Vec2<float> offset = { 0.0f, 0.0f });

public:

    LineEndType lineEndTypeStart = LineEndType::None;
    LineEndType lineEndTypeStop = LineEndType::None;

    // the length of a bracket end
    float endLengthStart = 0.0f;
    float endLengthStop = 0.0f;

    // whether this element is a MusicXML dashes element or  bracket element
    bool isDashes = false;

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

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> defaultPositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };

    Vec2<float> relativePositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif // MUSIQUE_BRACKETDIRECTION_H
