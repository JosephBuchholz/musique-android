#include "Lyric.h"

void Lyric::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{
    positionX = defX;
    positionY = -defY;

    positionX += relX;
    positionY -= relY;
}