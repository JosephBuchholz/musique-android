#ifndef MUSIQUE_BOWING_H
#define MUSIQUE_BOWING_H

#include "../VisibleElement.h"

class Bowing : VisibleElement
{
public:

    enum class Direction {
        None = 0, Up, Down
    } direction = Direction::None;
};

#endif // MUSIQUE_BOWING_H