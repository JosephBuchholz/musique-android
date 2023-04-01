#ifndef MUSIQUE_BOUNDINGBOX_H
#define MUSIQUE_BOUNDINGBOX_H

#include "Vec2.h"
#include "../RenderData/RenderData.h"
#include "../Utils/Converters.h"

class BoundingBox
{
public:

    /**
     * Calculates whether the given bounding box overlaps with this one.
     *
     * @param boundingBox The bounding box to check against.
     * @return Whether they overlap.
     */
    bool DoesOverlapWith(const BoundingBox& boundingBox) const;

    static bool DoBoundingBoxesOverlap(const BoundingBox& boundingBox1, const BoundingBox& boundingBox2);

    bool IsCloseTo(const BoundingBox& boundingBox, float minDistance) const;
    bool IsFarFrom(const BoundingBox& boundingBox, float maxDistance) const;

    void ResolveOverlap(BoundingBox boundingBox);

    /**
     * Resolve the overlap with out moving `this.`
     *
     * @param boundingBox The bounding box to resolve overlap with.
     * @return The x and y offset that was used to resolve the overlap.
     */
    Vec2<float> ResolveOverlapStatically(BoundingBox& boundingBox) const;

    void Render(RenderData& renderData, const int& color = 0xFFFF00FF) const;

    std::string GetPrintableString() const { return "position: " + position.GetPrintableString() + " | size: " + size.GetPrintableString(); }

public:

    bool operator==(const BoundingBox& rhs) const
    {
        return (this->position == rhs.position) && (this->size == rhs.size);
    }

public:

    // from the top left hand corner
    Vec2<float> position;

    Vec2<float> size;
};

#endif //MUSIQUE_BOUNDINGBOX_H