/**
 * This file contains the definition of the `Beam` class.
 */

#ifndef MUSIQUE_BEAM_H
#define MUSIQUE_BEAM_H

#include <vector>

#include "../Note.h"
#include "../NoteChord.h"
#include "../../BaseElements/VisibleElement.h"
#include "../../BaseElements/LineElement.h"

class Note;

class Beam : public VisibleElement, public LineElement
{
public:
    void Render(RenderData& renderData, Vec2<float> measurePosition, float slope, float yIntercept, bool isAboveNote, Vec2<float> offset = { 0.0f, 0.0f }) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param displayConstants The display constants.
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition);

    float GetPositionYOnBeam(float positionX, float slope, float yIntercept) const;

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants, float size);

public:
    std::vector<std::shared_ptr<Note>> notes; // the notes that this beam is associated with

    // relative to start of measure and top staff line
    Vec2<float> beamStartPosition = { 0.0f, 0.0f };
    Vec2<float> beamEndPosition = { 0.0f, 0.0f };

    float size = 1.0f;

    enum class BeamType {
        None = 0, Normal, ForwardHook, BackwardHook
    };

    BeamType beamType = BeamType::None;
    int beamLevel = 1;
};

#endif // MUSIQUE_BEAM_H