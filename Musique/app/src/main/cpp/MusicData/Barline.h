#pragma once
#include "VisibleElement.h"
#include "Types.h"

class Barline : public VisibleElement {

public:

    enum class Location {
        None = 0, Right, Left, Middle
    } location = Location::Right;

    enum class BarlineStyle {
        None = 0, Dashed, Dotted, Heavy, HeavyHeavy, HeavyLight, LightHeavy, LightLight, NoBarline, Regular, Short, Tick
    } barlineStyle = BarlineStyle::None;

    bool isRepeatBarLine = false;

    enum class Direction {
        None = 0, Backward, Forward
    };
    Direction facing = Direction::None;

    unsigned int repeatCount = 2;
};