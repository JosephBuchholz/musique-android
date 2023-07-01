/**
 * This file contains the definition of the `NoteStem` class.
 */

#ifndef MUSIQUE_NOTESTEM_H
#define MUSIQUE_NOTESTEM_H

#include <string>

#include "../BaseElements/VisibleElement.h"
#include "TremoloSingle.h"

/**
 * This class represents a note stem on a note.
 */
class NoteStem : public VisibleElement
{
public:

    void Render(RenderData& renderData, Vec2<float> notePosition, std::shared_ptr<TremoloSingle> tremoloSingle, bool isGraceNote, bool hasSlash, float noteHeadWidth, Vec2<float> offset = { 0.0f, 0.0f }) const;

    BoundingBox GetBoundingBoxRelativeToParent(const MusicDisplayConstants& displayConstants) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param displayConstants The display constants.
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition);

public:
    NoteStem() {}

    enum class StemType {
        None = 0, Up, Down, Double
    };

    static StemType CalculateStemTypeFromString(const std::string& string) {
        if (string == "up") {
            return StemType::Up;
        } else if (string == "down") {
            return StemType::Down;
        } else if (string == "double") {
            return StemType::Double;
        }
        return StemType::None;
    }

    StemType stemType = StemType::None;

    // relative to left side of note (or center of note for tablature)
    float stemPositionX = 0.0f;

    // relative to the center of the note
    float stemStartY = 0.0f; // closest to the note
    float stemEndY = 0.0f; // farthest away from the note
};

#endif // MUSIQUE_NOTESTEM_H