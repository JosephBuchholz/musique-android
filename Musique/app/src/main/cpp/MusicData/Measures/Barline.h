/**
 * This file contains the definition of the `Barline` class.
 */

#ifndef MUSIQUE_BARLINE_H
#define MUSIQUE_BARLINE_H

#include "../BaseElements/VisibleElement.h"
#include "../BaseElements/LineElement.h"
#include "../Types.h"
#include "../Directions/Words.h"

/**
 * This class represents a barline in a measure.
 */
class Barline : public VisibleElement, public LineElement
{
public:

    void Render(RenderData& renderData, Vec2<float> measurePosition, float height, float lineSpacing, int lineCount, bool isTopStaff, bool isPartOfEnding, Vec2<float> offset = { 0.0f, 0.0f }) const;

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants, float measureWidth, float repeatBarlineOffset);

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
    std::shared_ptr<Words> repeatNumberDirection = nullptr; // the number that appears above the end repeat symbol if 'repeatCount' > 2

    // relative to the measure
    float positionX = 0.0f;
};

#endif // MUSIQUE_BARLINE_H