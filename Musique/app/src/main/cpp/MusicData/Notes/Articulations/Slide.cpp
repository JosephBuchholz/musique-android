#include "Slide.h"

void Slide::Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX, float offsetY) const
{
    Paint slidePaint = Paint(color.color);
    slidePaint.strokeWidth = 1.25f;
    slidePaint.strokeCap = Paint::Cap::Butt;

    renderData.AddLine(std::make_shared<Line>(position.x + lineStartPosition.x + notePositionX + offsetX, position.y + lineStartPosition.y + notePositionY + offsetY, position.x + lineEndPosition.x + notePositionX + offsetX, position.y + lineEndPosition.y + notePositionY + offsetY, slidePaint));
}

Vec2<float> Slide::GetDimensions() const
{
    return { std::abs(lineEndPosition.x - lineStartPosition.x), std::abs(lineEndPosition.y - lineStartPosition.y) };
}

void Slide::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab)
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