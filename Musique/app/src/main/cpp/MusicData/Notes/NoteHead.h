/**
 * This file contains the definition of the 'NoteHead' class.
 */

#ifndef MUSIQUE_NOTEHEAD_H
#define MUSIQUE_NOTEHEAD_H

#include "../VisibleElement.h"

/**
 * This class represents a note head for a note.
 */
class NoteHead : public VisibleElement
{
public:

    void Render(RenderData& renderData, Vec2<float> position, NoteValue noteDuration, bool centerHorizontally = false, Vec2<float> offset = { 0.0f, 0.0f }) const;

    Vec2<float> GetDimensions(NoteValue noteDuration) const;

    SMuFLID GetSMuFLID(NoteValue noteDuration) const;

public:

    enum class NoteHeadType {
        None = 0, Normal, NoNoteHead, Other, ArrowDown, ArrowUp, BackSlashed,
        CircleDot, CircleX, Circled, Cluster, Cross, Diamond, InvertedTriangle,
        LeftTriangle, Rectangle, Slash, Slashed, Square, Triangle, X,
        Do, Re, Mi, Fa, FaUp, So, La, Ti
    } type = NoteHeadType::Normal;

    bool filled = false;
    bool hasParentheses = false;
};

#endif //MUSIQUE_NOTEHEAD_H
