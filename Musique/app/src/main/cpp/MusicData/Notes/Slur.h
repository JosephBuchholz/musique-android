/**
 * This file contains the definition of the `Slur` class.
 */

#ifndef MUSIQUE_SLUR_H
#define MUSIQUE_SLUR_H

#include "../BaseElements/VisibleElement.h"
#include "../BaseElements/LineElement.h"
#include "../Types.h"
#include "Note.h"

/**
 * This class represents a musical slur.
 */
class Slur : public VisibleElement, public LineElement
{
    friend class Measure;

public:

    void Render(RenderData& renderData, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:

    std::vector<std::shared_ptr<Note>> notes; // the notes that this slur is associated with

    AboveBelowType placement = AboveBelowType::None;
    CurveOrientation orientation = CurveOrientation::Over;

    int startMeasureIndex; // the measure index at which this slur starts
    int endMeasureIndex; // the measure index at which this slur ends

    Vec2<float> startPosition = { 0.0f, 0.0f };
    Vec2<float> endPosition = { 0.0f, 0.0f };

    bool isBroken = false; // whether the slur is broken across two different systems
};

#endif // MUSIQUE_SLUR_H