#ifndef MUSIQUE_BOUNDINGBOX_H
#define MUSIQUE_BOUNDINGBOX_H

#include "Vec2.h"
#include "../RenderData/RenderData.h"
#include "../Utils/Converters.h"

#define DEFAULT_PADDING 2.0f

class Constraint
{
public:

    enum class ConstraintType
    {
        None = 0,       /** No Constraint */
        Static,         /** Cannot be moved (has every constraint) */
        NoHorizontal,   /** Cannot be moved horizontally */
        NoVertical,     /** Cannot be moved vertically */
        NoRotation,     /** Cannot be rotated */
        NoResize,       /** Cannot be resized */
        NoResizeWidth,  /** The width cannot be resized */
        NoResizeHeight, /** The height cannot be resized */
    };

    Constraint() {}
    Constraint(ConstraintType type)
        : constraintType(type) {}

public:

    bool operator==(const Constraint& rhs)
    {
        return (this->constraintType == rhs.constraintType);
    }

public:
    ConstraintType constraintType = ConstraintType::None;
};

class BoundingBox
{
public:

    BoundingBox();

    float GetLeft() const { return position.x; }
    float GetRight() const { return position.x + size.x; }

    float GetTop() const { return position.y; }
    float GetBottom() const { return position.y + size.y; }

    /**
     * Calculates whether the given bounding box overlaps with this one.
     *
     * @param boundingBox The bounding box to check against.
     * @return Whether they overlap.
     */
    bool DoesOverlapWith(const BoundingBox& boundingBox) const;

    /**
     * Finds whether the given point overlaps with this bounding box.
     *
     * @param point The point to test against.
     * @return Whether the point overlaps.
     */
    bool DoesOverlapWithPoint(Vec2<float> point) const;

    static bool DoBoundingBoxesOverlap(const BoundingBox& boundingBox1, const BoundingBox& boundingBox2);

    bool IsCloseTo(const BoundingBox& boundingBox, float minDistance) const;
    bool IsFarFrom(const BoundingBox& boundingBox, float maxDistance) const;


    static BoundingBox CombineBoundingBoxes(const BoundingBox& boundingBox1, const BoundingBox& boundingBox2);

    static void ResolveOverlap(BoundingBox& boundingBox1, BoundingBox& boundingBox2);

    /**
     * Resolve the overlap with out moving `this.`
     *
     * @param boundingBox The bounding box to resolve overlap with.
     * @return The x and y offset that was used to resolve the overlap.
     */
    Vec2<float> ResolveOverlapStatically(BoundingBox& boundingBox) const;

    void ResolveOverlapWithNoConstraints(BoundingBox& boundingBox);

    bool IsStatic() const;
    bool HasNoConstraints() const;
    bool HasConstraint(Constraint constraint) const;

    /**
     * Adds the given amount of padding to the outside edges.
     *
     * @param padding The amount of padding.
     */
    BoundingBox AddPadding(float padding = DEFAULT_PADDING);

    /**
     * Makes the dimensions of this bounding box positive while adjusting
     * the position so that the bounding box is in the spot.
     */
    void MakeDimensionsPositive();

    void Render(RenderData& renderData, Color color = Color(0xFFFF00FF), Vec2<float> offset = { 0.0f, 0.0f }) const;

    /**
     * Debug only.
     *
     * @return The string.
     */
    std::string GetPrintableString() const { return "position: " + position.GetPrintableString() + " | size: " + size.GetPrintableString(); }

    /**
     * Debug only.
     *
     * @return The string.
     */
    std::string GetPrintableConstraints() const;

public:

    bool operator==(const BoundingBox& rhs) const
    {
        return (this->position == rhs.position) && (this->size == rhs.size);
    }

public:

    // from the top left hand corner
    Vec2<float> position = { 0.0f, 0.0f };
    Vec2<float> size = { 0.0f, 0.0f };

    std::vector<Constraint> constraints;
};

#endif //MUSIQUE_BOUNDINGBOX_H