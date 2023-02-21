#include "AugmentationDot.h"

void AugmentationDot::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, bool noteIsOnLine)
{
    positionY = 0.0f;
    positionX = 0.0f;

    if (noteIsOnLine) // if the dot is on a staff line
    {
        if (placement == AboveBelowType::Above)
            positionY -= displayConstants.lineSpacing * 0.5f;
        else if (placement == AboveBelowType::Below)
            positionY += displayConstants.lineSpacing * 0.5f;
    }

    positionX += 15.0f;
}