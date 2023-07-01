/**
 * This file contains the definition of the `BeamGroup` class.
 */

#ifndef MUSIQUE_BEAMGROUP_H
#define MUSIQUE_BEAMGROUP_H

#include <vector>

class Beam;

#include "Beam.h"
#include "../../BaseElements/VisibleElement.h"
#include "../../BaseElements/LineElement.h"

class BeamGroup : public VisibleElement, public LineElement
{
public:

    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;
    void RenderDebug(RenderData& renderData) const;

    BoundingBox GetBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param displayConstants The display constants.
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition);

    float GetPositionYOnBeam(float positionX) const;

    float GetSlope() const;

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants);

public:

    std::vector<std::shared_ptr<Note>> notes; // the notes that this beam group is associated with

    std::vector<Beam> beams;

    // relative to start of measure and top staff line
    Vec2<float> beamStartPosition = { 0.0f, 0.0f };
    Vec2<float> beamEndPosition = { 0.0f, 0.0f };

    bool isAboveNote = false;
};

#endif // MUSIQUE_BEAMGROUP_H