#include "Words.h"

void Words::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY)
{
    /*if (noDefX)
        positionX = defaultX + relX;
    else
        positionX = defX + relX;*/

    if (noDefY)
        positionY = defaultY + -relY;
    else
        positionY = -defY + -relY;

    positionX = defaultX;
    //positionY = defaultY;
}