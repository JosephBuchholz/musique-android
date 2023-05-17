/**
 * This file contains the definition of the 'TremoloSingle' class.
 */

#ifndef MUSIQUE_TREMOLOSINGLE_H
#define MUSIQUE_TREMOLOSINGLE_H

#include "../BaseElements/VisibleElement.h"

/**
 * This class represents a single note tremolo
 * (tremolos that contain only one note (including buzz rolls),
 * not double note tremolos).
 */
class TremoloSingle : public VisibleElement
{
    friend class Note;

public:

    void Render(RenderData& renderData, Vec2<float> notePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    SMuFLID GetSMuFLID() const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:

    enum class Type
    {
        None = 0, Normal, Unmeasured, BuzzRoll
    } type = Type::Normal;

    int tremoloMarks = 0;

    Vec2<float> position = { 0.0f, 0.0f };
};

#endif //MUSIQUE_TREMOLOSINGLE_H
