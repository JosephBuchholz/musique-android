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

    void Render(RenderData& renderData) const;

    std::string GetPrintableString() const { return "position: " + position.GetPrintableString() + " | size: " + size.GetPrintableString(); }

public:

    // from the top left hand corner
    Vec2<float> position;

    Vec2<float> size;
};

#endif //MUSIQUE_BOUNDINGBOX_H