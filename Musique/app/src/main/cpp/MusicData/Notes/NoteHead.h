/**
 * This file contains the definition of the 'NoteHead' class.
 */

#ifndef MUSIQUE_NOTEHEAD_H
#define MUSIQUE_NOTEHEAD_H

#include "../BaseElements/VisibleElement.h"

/**
 * This class represents a note head for a note.
 */
class NoteHead : public VisibleElement
{
    friend class Note;

public:

    void Render(RenderData& renderData, Vec2<float> position, bool centerHorizontally = false, Vec2<float> offset = { 0.0f, 0.0f }) const;

    Vec2<float> GetDimensions(const MusicDisplayConstants& displayConstants) const;

    SMuFLID GetSMuFLID() const;
    Paint GetPaint(const MusicDisplayConstants& displayConstants) const;

    float GetNoteHeadWidth(const MusicDisplayConstants& displayConstants) const;

    float GetCenterPositionX(const MusicDisplayConstants& displayConstants) const;

    BoundingBox GetBoundingBoxRelativeToParent(const MusicDisplayConstants& displayConstants) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param displayConstants The display constants.
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition);

protected:

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants, NoteSize noteSize);

public:

    NoteType noteType = NoteType::Standard;
    NoteValue noteDuration = NoteValue::None;

    enum class NoteHeadType {
        None = 0, Normal, NoNoteHead, Other, ArrowDown, ArrowUp, BackSlashed,
        CircleDot, CircleX, Circled, Cluster, Cross, Diamond, InvertedTriangle,
        LeftTriangle, Rectangle, Slash, Slashed, Square, Triangle, X,
        Do, Re, Mi, Fa, FaUp, So, La, Ti
    } type = NoteHeadType::Normal;

    bool filled = false;
    bool hasParentheses = false;

    // tab only
    int fret = 0;
};

#endif //MUSIQUE_NOTEHEAD_H
