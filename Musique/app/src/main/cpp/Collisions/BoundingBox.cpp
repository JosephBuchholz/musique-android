#include "BoundingBox.h"

#include "../Debuging/AndroidDebug.h"

bool BoundingBox::DoesOverlapWith(const BoundingBox& boundingBox) const
{
    return (this->position.x < boundingBox.position.x + boundingBox.size.x &&
            this->position.x + this->size.x > boundingBox.position.x &&
            this->position.y < boundingBox.position.y + boundingBox.size.y &&
            this->position.y + this->size.y > boundingBox.position.y);
}

bool BoundingBox::DoesOverlapWithPoint(Vec2<float> point) const
{
    return (point.x > this->position.x &&
            point.x < this->position.x + this->size.x &&
            point.y > this->position.y &&
            point.y < this->position.y + this->size.y);
}

bool BoundingBox::DoBoundingBoxesOverlap(const BoundingBox& boundingBox1, const BoundingBox& boundingBox2)
{
    return boundingBox1.DoesOverlapWith(boundingBox2);
}

bool BoundingBox::IsCloseTo(const BoundingBox& boundingBox, float minDistance) const
{
    // TODO: implement
    LOGE("\'IsCloseTo\' function is not implemented");
    return false;
}

bool BoundingBox::IsFarFrom(const BoundingBox& boundingBox, float maxDistance) const
{
    // TODO: implement
    LOGE("\'IsFarFrom\' function is not implemented");
    return false;
}

void BoundingBox::ResolveOverlap(BoundingBox boundingBox)
{
    // TODO: implement
    LOGE("\'ResolveOverlap\' function is not implemented");
}

BoundingBox BoundingBox::CombineBoundingBoxes(const BoundingBox& boundingBox1, const BoundingBox& boundingBox2)
{
    BoundingBox newBoundingBox;

    float left = std::min(boundingBox1.position.x, boundingBox2.position.x);
    float top = std::min(boundingBox1.position.y, boundingBox2.position.y);

    float right = std::max(boundingBox1.position.x + boundingBox1.size.x, boundingBox2.position.x + boundingBox2.size.x);
    float bottom = std::max(boundingBox1.position.y + boundingBox1.size.y, boundingBox2.position.y + boundingBox2.size.y);

    newBoundingBox.position.x = left;
    newBoundingBox.position.y = top;

    newBoundingBox.size.x = right - left;
    newBoundingBox.size.y = bottom - top;

    return newBoundingBox;
}

Vec2<float> BoundingBox::ResolveOverlapStatically(BoundingBox& boundingBox) const
{
    Vec2<float> overlapOffset;

    BoundingBox oldBoundingBox = boundingBox;

    if (*this == boundingBox)
        return overlapOffset;

    if (DoBoundingBoxesOverlap(*this, boundingBox))
    {
        float overlapX = 0.0f;

        if (this->position.x > boundingBox.position.x)
            overlapX = (boundingBox.position.x + boundingBox.size.x) - this->position.x;
        else if (this->position.x < boundingBox.position.x)
            overlapX = (this->position.x + this->size.x) - boundingBox.position.x;

        float overlapY = 0.0f;

        if (this->position.y > boundingBox.position.y)
            overlapY = (boundingBox.position.y + boundingBox.size.y) - this->position.y;
        else if (this->position.y < boundingBox.position.y)
            overlapY = (this->position.y + this->size.y) - boundingBox.position.y;

        if (overlapX < overlapY)
        {
            if (this->position.x > boundingBox.position.x)
                boundingBox.position.x -= overlapX - 3.0f;
            else if (this->position.x < boundingBox.position.x)
                boundingBox.position.x += overlapX + 3.0f;
        }
        else
        {
            if (this->position.y > boundingBox.position.y)
                boundingBox.position.y -= overlapY - 3.0f;
            else if (this->position.y < boundingBox.position.y)
                boundingBox.position.y += overlapY + 3.0f;
        }
    }

    if (DoBoundingBoxesOverlap(*this, boundingBox))
        LOGE("Bounding boxes still overlap");

    /*while (DoBoundingBoxesOverlap(*this, boundingBox))
    {
        boundingBox.position.y -= 5.0f;
    }*/

    overlapOffset = boundingBox.position - oldBoundingBox.position;

    //if (overlapOffset.x != 0 || overlapOffset.y != 0)
    //    LOGE("overlap offset: %s", overlapOffset.GetPrintableString().c_str());

    return overlapOffset;
}

void BoundingBox::AddPadding(float padding)
{
    position.x -= padding;
    position.y -= padding;
    size.x += padding * 2.0f;
    size.y += padding * 2.0f;
}

void BoundingBox::MakeDimensionsPositive()
{
    if (size.x < 0.0f)
    {
        size.x = -size.x; // make positive
        position.x -= size.x; // adjust position
    }

    // same as x
    if (size.y < 0.0f)
    {
        size.y = -size.y;
        position.y -= size.y;
    }
}

void BoundingBox::Render(RenderData& renderData, const int& color) const
{
    Paint paint = Paint();
    paint.color = color;
    paint.strokeWidth = 2.0f;

    // horizontal lines
    renderData.AddLine(position.x, position.y, position.x + size.x, position.y, paint);
    renderData.AddLine(position.x, position.y + size.y, position.x + size.x, position.y + size.y, paint);

    // vertical lines
    renderData.AddLine(position.x, position.y, position.x, position.y + size.y, paint);
    renderData.AddLine(position.x + size.x, position.y, position.x + size.x, position.y + size.y, paint);
}