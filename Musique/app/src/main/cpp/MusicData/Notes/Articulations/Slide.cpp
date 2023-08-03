#include "Slide.h"

void Slide::Render(RenderData& renderData, Vec2<float> notePosition) const
{
    Paint slidePaint = Paint(color.color);
    slidePaint.strokeWidth = 1.25f;
    slidePaint.strokeCap = Paint::Cap::Butt;

    renderData.AddLine(Line({ position.x + lineStartPosition.x + notePosition.x, position.y + lineStartPosition.y + notePosition.y }, { position.x + lineEndPosition.x + notePosition.x, position.y + lineEndPosition.y + notePosition.y }, slidePaint));
}

BoundingBox Slide::GetBoundingBox() const
{
    BoundingBox bb;

    bb.size = GetDimensions();
    bb.position += position;

    return bb;
}

Vec2<float> Slide::GetDimensions() const
{
    return { std::abs(lineEndPosition.x - lineStartPosition.x), std::abs(lineEndPosition.y - lineStartPosition.y) };
}

void Slide::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY)
{
    position = { 0.0f, 0.0f };

    switch (type)
    {
        case Type::Scoop:
        {
            lineStartPosition = { 0.0f, 0.0f };
            lineEndPosition = { -15.0f, 10.0f };
            break;
        }
        case Type::Plop:
        {
            lineStartPosition = { 0.0f, 0.0f };
            lineEndPosition = { -15.0f, -10.0f };
            break;
        }
        case Type::Doit:
        {
            lineStartPosition = { 0.0f, 0.0f };
            lineEndPosition = { 15.0f, -10.0f };
            break;
        }
        case Type::Falloff:
        {
            lineStartPosition = { 0.0f, 0.0f };
            lineEndPosition = { 15.0f, 10.0f };
            break;
        }
        default:
        {
            lineStartPosition = { 0.0f, 0.0f };
            lineEndPosition = { 0.0f, 0.0f };
            break;
        }
    }
}