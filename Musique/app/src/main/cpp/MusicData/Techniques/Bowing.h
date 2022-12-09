#pragma once


#include "../VisibleElement.h"

class Bowing : VisibleElement
{
public:

    enum class Direction {
        None = 0, Up, Down
    } direction = Direction::None;
};