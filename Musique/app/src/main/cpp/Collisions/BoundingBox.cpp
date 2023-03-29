#include "BoundingBox.h"

#include "../Debuging/AndroidDebug.h"

bool BoundingBox::DoesOverlapWith(const BoundingBox& boundingBox) const
{
    return (this->position.x < boundingBox.position.x + boundingBox.size.x &&
            this->position.x + this->size.x > boundingBox.position.x &&
            this->position.y < boundingBox.position.y + boundingBox.size.y &&
            this->position.y + this->size.y > boundingBox.position.y);
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

void BoundingBox::Render(RenderData& renderData) const
{
    Paint paint = Paint();
    paint.color = (int)0xFFFF00FF;
    paint.strokeWidth = 2.0f;

    // horizontal lines
    renderData.AddLine(position.x, position.y, position.x + size.x, position.y, paint);
    renderData.AddLine(position.x, position.y + size.y, position.x + size.x, position.y + size.y, paint);

    // vertical lines
    renderData.AddLine(position.x, position.y, position.x, position.y + size.y, paint);
    renderData.AddLine(position.x + size.x, position.y, position.x + size.x, position.y + size.y, paint);
}