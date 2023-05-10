/**
 * This file contains the definition of the `Barline` class.
 */

#ifndef MUSIQUE_BARLINE_H
#define MUSIQUE_BARLINE_H

#include "../BaseElements/VisibleElement.h"
#include "../BaseElements/LineElement.h"
#include "../Types.h"

/**
 * This class represents a barline in a measure.
 */
class Barline : public VisibleElement, public LineElement {

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

#endif // MUSIQUE_BARLINE_H