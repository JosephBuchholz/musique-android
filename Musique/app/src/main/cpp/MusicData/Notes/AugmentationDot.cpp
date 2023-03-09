#include "AugmentationDot.h"

void AugmentationDot::Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX, float offsetY) const
{
    renderData.AddGlyph(SMuFLGlyph(SMuFLID::augmentationDot, positionX + notePositionX + offsetY, positionY + notePositionY + offsetX, Paint(color.color)));
}

void AugmentationDot::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, bool noteIsOnLine, bool isTab)
{
    positionY = 0.0f;
    positionX = 0.0f;

    if (isTab)
    {
        if (placement == AboveBelowType::Above)
            positionY -= displayConstants.tabLineSpacing * 0.5f;
        else if (placement == AboveBelowType::Below)
            positionY += displayConstants.tabLineSpacing * 0.5f;

        positionX += 10.0f;
    }
    else
    {
        if (noteIsOnLine) // if the dot is on a staff line
        {
            if (placement == AboveBelowType::Above)
                positionY -= displayConstants.lineSpacing * 0.5f;
            else if (placement == AboveBelowType::Below)
                positionY += displayConstants.lineSpacing * 0.5f;
        }

        positionX += 15.0f;
    }
}