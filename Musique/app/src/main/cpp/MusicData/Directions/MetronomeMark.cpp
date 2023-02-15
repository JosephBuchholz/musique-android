#include "MetronomeMark.h"

void MetronomeMark::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY)
{
    if (noDefX)
        positionX = defaultX + relX;
    else
        positionX = defX + relX;

    if (noDefY)
        positionY = defaultY + -relY;
    else
        positionY = -defY + -relY;
}