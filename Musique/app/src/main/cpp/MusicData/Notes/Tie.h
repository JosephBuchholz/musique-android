/**
 * This file contains the definition of the `NoteTie` class.
 */

#ifndef MUSIQUE_TIE_H
#define MUSIQUE_TIE_H

#include "../BaseElements/VisibleElement.h"
#include "../BaseElements/LineElement.h"
#include "../Types.h"

class NoteTie;

#include "Note.h"

/**
 * Class that represents a tie.
 */
class NoteTie : public VisibleElement, public LineElement
{
    friend class MusicXMLParser;

public:

    void Render(RenderData& renderData, Vec2<float> startNotePosition, Vec2<float> endNotePosition, bool isFirstNote) const;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd, AboveBelowType defaultPlacement, bool isNoteChord);

private:
    void RenderCurve(RenderData& renderData, Vec2<float> startPosition, Vec2<float> endPosition) const;

public:

    CurveOrientation orientation = CurveOrientation::Over; // whether the tie is overhand (tips down) or underhand (tips up)

    AboveBelowType placement = AboveBelowType::Above; // whether the tie appears above or below the note

    std::pair<std::shared_ptr<Note>, std::shared_ptr<Note>> notes; // the notes associated with this tie

    // -- Positioning Attributes --

    // relative to the starting note
    Vec2<float> positionStart = { 0.0f, 0.0f };

    // relative to the ending note
    Vec2<float> positionEnd = { 0.0f, 0.0f };

    bool isBroken = false; // whether this tie is broken across two systems (starts in one system and ends in another system)
};

#endif // MUSIQUE_TIE_H