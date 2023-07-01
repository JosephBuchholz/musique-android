#include "BoundingBox.h"

#include "../Debuging/AndroidDebug.h"

BoundingBox::BoundingBox()
{

}

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

void BoundingBox::ResolveOverlap(BoundingBox& boundingBox1, BoundingBox& boundingBox2)
{
    if (!DoBoundingBoxesOverlap(boundingBox1, boundingBox2) || boundingBox1 == boundingBox2)
        return;

    if (boundingBox1.IsStatic())
    {
        boundingBox1.ResolveOverlapStatically(boundingBox2);
    }
    else if (boundingBox2.IsStatic())
    {
        boundingBox2.ResolveOverlapStatically(boundingBox1);
    }
    else if (boundingBox1.HasNoConstraints())
    {
        boundingBox1.ResolveOverlapWithNoConstraints(boundingBox2);
    }
    else if (boundingBox2.HasNoConstraints())
    {
        boundingBox2.ResolveOverlapWithNoConstraints(boundingBox1);
    }
    else
    {
        float errorValue = 0.0f;
        if (boundingBox1.HasConstraint(Constraint::ConstraintType::NoVertical))
        {
            float overlapX = 0.0f;
            if (boundingBox1.position.x > boundingBox2.position.x)
                overlapX = (boundingBox2.position.x + boundingBox2.size.x) - boundingBox1.position.x;
            else if (boundingBox1.position.x < boundingBox2.position.x)
                overlapX = (boundingBox1.position.x + boundingBox1.size.x) - boundingBox2.position.x;
            else
                overlapX = boundingBox1.size.x;

            if (boundingBox1.position.x >= boundingBox2.position.x)
                boundingBox1.position.x += overlapX + errorValue;
            else if (boundingBox1.position.x < boundingBox2.position.x)
                boundingBox1.position.x -= overlapX + errorValue;
        }
        else if (boundingBox1.HasConstraint(Constraint::ConstraintType::NoHorizontal))
        {
            float overlapY = 0.0f;
            if (boundingBox1.position.y > boundingBox2.position.y)
                overlapY = (boundingBox2.position.y + boundingBox2.size.y) - boundingBox1.position.y;
            else if (boundingBox1.position.y < boundingBox2.position.y)
                overlapY = (boundingBox1.position.y + boundingBox1.size.y) - boundingBox2.position.y;
            else
                overlapY = boundingBox1.size.y;

            if (boundingBox1.position.y >= boundingBox2.position.y)
                boundingBox1.position.y += overlapY + errorValue;
            else if (boundingBox1.position.y < boundingBox2.position.y)
                boundingBox1.position.y -= overlapY + errorValue;
        }
    }

    bool bothStatic = (boundingBox1.IsStatic() && boundingBox2.IsStatic());
    if (DoBoundingBoxesOverlap(boundingBox1, boundingBox2))
    {
        if (!bothStatic)
        {
            LOGE("Bounding boxes still overlap!!!!!!! bothStatic: %d", bothStatic);
            LOGD("boundingBox1: %s, %s", boundingBox1.GetPrintableConstraints().c_str(), boundingBox1.GetPrintableString().c_str());
            LOGD("boundingBox2: %s, %s", boundingBox2.GetPrintableConstraints().c_str(), boundingBox2.GetPrintableString().c_str());
        }
    }
}

Vec2<float> BoundingBox::ResolveOverlapStatically(BoundingBox& boundingBox) const
{
    Vec2<float> overlapOffset = { 0.0f, 0.0f };

    BoundingBox oldBoundingBox = boundingBox;

    if (*this == boundingBox || boundingBox.IsStatic())
        return overlapOffset;

    if (DoBoundingBoxesOverlap(*this, boundingBox))
    {
        float overlapX = 0.0f;

        if (this->position.x > boundingBox.position.x)
            overlapX = (boundingBox.position.x + boundingBox.size.x) - this->position.x;
        else if (this->position.x < boundingBox.position.x)
            overlapX = (this->position.x + this->size.x) - boundingBox.position.x;
        else
            overlapX = this->size.y;

        float overlapY = 0.0f;

        if (this->position.y > boundingBox.position.y)
            overlapY = (boundingBox.position.y + boundingBox.size.y) - this->position.y;
        else if (this->position.y < boundingBox.position.y)
            overlapY = (this->position.y + this->size.y) - boundingBox.position.y;
        else
            overlapY = this->size.y;

        float errorValue = 0.0f;
        if (boundingBox.HasNoConstraints())
        {
            if (overlapX < overlapY)
            {
                if (this->position.x >= boundingBox.position.x)
                    boundingBox.position.x -= overlapX - errorValue;
                else if (this->position.x < boundingBox.position.x)
                    boundingBox.position.x += overlapX + errorValue;
            }
            else
            {
                if (this->position.y >= boundingBox.position.y)
                    boundingBox.position.y -= overlapY - errorValue;
                else if (this->position.y < boundingBox.position.y)
                    boundingBox.position.y += overlapY + errorValue;
            }
        }
        else if (boundingBox.HasConstraint(Constraint::ConstraintType::NoHorizontal))
        {
            if (this->position.y >= boundingBox.position.y)
                boundingBox.position.y -= overlapY + errorValue;
            else if (this->position.y < boundingBox.position.y)
                boundingBox.position.y += overlapY + errorValue;
        }
        else if (boundingBox.HasConstraint(Constraint::ConstraintType::NoVertical))
        {
            if (this->position.x >= boundingBox.position.x)
                boundingBox.position.x -= overlapX + errorValue;
            else if (this->position.x < boundingBox.position.x)
                boundingBox.position.x += overlapX + errorValue;
        }
    }

    //if (DoBoundingBoxesOverlap(*this, boundingBox))
    //    LOGE("Bounding boxes still overlap");

    /*while (DoBoundingBoxesOverlap(*this, boundingBox))
    {
        boundingBox.position.y -= 5.0f;
    }*/

    overlapOffset = boundingBox.position - oldBoundingBox.position;

    //if (overlapOffset.x != 0 || overlapOffset.y != 0)
    //    LOGE("overlap offset: %s", overlapOffset.GetPrintableString().c_str());

    return overlapOffset;
}

void BoundingBox::ResolveOverlapWithNoConstraints(BoundingBox& boundingBox)
{
    if (!DoBoundingBoxesOverlap(*this, boundingBox) || *this == boundingBox)
        return;

    float overlapX = 0.0f;

    if (this->position.x > boundingBox.position.x)
        overlapX = (boundingBox.position.x + boundingBox.size.x) - this->position.x;
    else if (this->position.x < boundingBox.position.x)
        overlapX = (this->position.x + this->size.x) - boundingBox.position.x;
    else
        overlapX = this->size.x;

    float overlapY = 0.0f;

    if (this->position.y > boundingBox.position.y)
        overlapY = (boundingBox.position.y + boundingBox.size.y) - this->position.y;
    else if (this->position.y < boundingBox.position.y)
        overlapY = (this->position.y + this->size.y) - boundingBox.position.y;
    else
        overlapY = this->size.y;

    //LOGV("overlapX %f, overlapY %f, this->pos: %s", overlapX, overlapY, this->position.GetPrintableString().c_str());

    float errorValue = 0.0f;
    if (overlapX < overlapY)
    {
        if (boundingBox.HasNoConstraints() || !boundingBox.HasConstraint(Constraint::ConstraintType::NoHorizontal))
        {
            overlapX /= 2.0f;

            if (this->position.x > boundingBox.position.x)
            {
                this->position.x += overlapX + errorValue;
                boundingBox.position.x -= overlapX + errorValue;
            }
            else if (this->position.x < boundingBox.position.x)
            {
                this->position.x -= overlapX + errorValue;
                boundingBox.position.x += overlapX + errorValue;
            }
            else
            {
                boundingBox.position.x += overlapX;
                boundingBox.position.x -= overlapX;
            }
        }
        else
        {
            if (this->position.x > boundingBox.position.x)
                this->position.x += overlapX + errorValue;
            else if (this->position.x < boundingBox.position.x)
                this->position.x -= overlapX + errorValue;
            else
                this->position.x += overlapX;
        }
    }
    else
    {
        if (boundingBox.HasNoConstraints() || !boundingBox.HasConstraint(Constraint::ConstraintType::NoVertical))
        {
            overlapY /= 2.0f;

            if (this->position.y > boundingBox.position.y)
            {
                this->position.y += overlapY + errorValue;
                boundingBox.position.y -= overlapY + errorValue;
            }
            else if (this->position.y < boundingBox.position.y)
            {
                this->position.y -= overlapY + errorValue;
                boundingBox.position.y += overlapY + errorValue;
            }
            else
            {
                boundingBox.position.y += overlapY;
                boundingBox.position.y -= overlapY;
            }
        }
        else
        {
            if (this->position.y > boundingBox.position.y)
                this->position.y += overlapY + errorValue;
            else if (this->position.y < boundingBox.position.y)
                this->position.y -= overlapY + errorValue;
            else
                this->position.y += overlapY;
        }
    }

    //LOGV("this->pos: %s", this->position.GetPrintableString().c_str());
}

bool BoundingBox::IsStatic() const
{
    return HasConstraint(Constraint::ConstraintType::Static);
}

bool BoundingBox::HasNoConstraints() const
{
    if (constraints.empty())
        return true;

    // all of the constraints are of 'None' type
    bool hasNoConstraints = true;
    for (auto constraint : constraints)
    {
        hasNoConstraints &= (constraint == Constraint::ConstraintType::None);
    }
    return hasNoConstraints;
}

bool BoundingBox::HasConstraint(Constraint constraint) const
{
    for (auto c : constraints)
    {
        if (c == constraint)
            return true;
    }

    return false;
}

BoundingBox BoundingBox::AddPadding(float padding)
{
    position.x -= padding;
    position.y -= padding;
    size.x += padding * 2.0f;
    size.y += padding * 2.0f;

    return *this;
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

std::string BoundingBox::GetPrintableConstraints() const
{
    std::string s = "| Constraints: ";

    for (auto constraint : constraints)
    {
        switch (constraint.constraintType)
        {
            case Constraint::ConstraintType::None: s += "'None'"; break;
            case Constraint::ConstraintType::Static: s += "'Static'"; break;
            case Constraint::ConstraintType::NoHorizontal: s += "'NoHorizontal'"; break;
            case Constraint::ConstraintType::NoVertical: s += "'NoVertical'"; break;
            case Constraint::ConstraintType::NoRotation: s += "'NoRotation'"; break;
            case Constraint::ConstraintType::NoResize: s += "'NoResize'"; break;
            case Constraint::ConstraintType::NoResizeWidth: s += "'NoResizeWidth'"; break;
            case Constraint::ConstraintType::NoResizeHeight: s += "'NoResizeHeight'"; break;
            default: s += "'ERROR'"; break;
        }

        s += ", ";
    }

    s += " |";

    return s;
}