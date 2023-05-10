/**
 * This file contains the definition of the `Tuplet` class.
 */

#ifndef MUSIQUE_TUPLET_H
#define MUSIQUE_TUPLET_H

#include "../BaseElements/VisibleElement.h"
#include "../../Collisions/Vec2.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/RenderData.h"
#include "../Notes/Note.h"

/**
 * This class represents a tuplet (such as a triplet or duplet).
 */
class Tuplet : public VisibleElement {
    friend class Song;
    friend class MusicXMLParser;

public:

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float> &parentPosition);

    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    SMuFLID GetSMuFLID() const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd);

public:

    enum class TupletType {
        None = 0, Duplet, Triplet, Quadrulpet, Quintuplet, Sextuplet, Septuplet, Octuplet, Nonuplet
    } tupletType = TupletType::Triplet;

    std::vector<Note*> notes; // pointers to the notes that this tuplet is associated with

    bool hasBracket = false;

    int actualNotes;
    int normalNotes;

    float endLengthHeight = 8.0f;

    // -- Positioning Attributes --

    // relative to measure
    Vec2<float> positionStart = { 0.0f, 0.0f };
    Vec2<float> positionEnd = { 0.0f, 0.0f };
};

#endif //MUSIQUE_TUPLET_H