/**
 * This file contains the definition for the 'Arpeggio' class.
 */

#ifndef MUSIQUE_ARPEGGIO_H
#define MUSIQUE_ARPEGGIO_H

#include "../BaseElements/VisibleElement.h"

#include "../ComplexLine.h"

#include "../Notes/Note.h"

/**
 * This class represents a musical arpeggio symbol (including strums and non-arpeggio brackets).
 */
class Arpeggio : public VisibleElement
{
    friend class Measure;
    friend class MusicXMLParser;

public:

    void Render(RenderData& renderData, float notePositionX, float measurePositionY, Vec2<float> offset = { 0.0f, 0.0f }) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd);

public:

    enum class ArpeggioType {
        None = 0, Arpeggio, ArpeggioUpArrow, ArpeggioDownArrow, StrumUpArrow, StrumDownArrow, NonArpeggio
    } type = ArpeggioType::Arpeggio;

    std::vector<std::shared_ptr<Note>> notes; // pointers to the notes that this arpeggio is associated with (the first note is the bottom note of the arpeggio)

    std::shared_ptr<ComplexLine> complexLine;

    // -- Positioning Attributes --

    // y is relative to top staff line measure
    // x is relative to the note
    Vec2<float> positionStart = { 0.0f, 0.0f };
    Vec2<float> positionEnd = { 0.0f, 0.0f };

    bool isBroken = false; // whether this arpeggio is broken across multiple staves

protected:

    // -- Positioning Attributes From MusicXML --

    Vec2<float> defaultPositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> defaultPositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };

    Vec2<float> relativePositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif //MUSIQUE_ARPEGGIO_H
