/**
 * This file contains the definition of the `TABSlur` class.
 */

#ifndef MUSIQUE_TABSLUR_H
#define MUSIQUE_TABSLUR_H

#include "../BaseElements/VisibleElement.h"
#include "../Types.h"
#include <string>

#include "Note.h"

/**
 * This class represents a guitar hammer-on or pull-off.
 */
class TABSlur : public VisibleElement
{
public:

    void Render(RenderData& renderData, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:

    //std::vector<std::shared_ptr<Note>> notes; // the notes that this class is associated with

    enum class SlurType {
        None = 0, HammerOn, PullOff
    } slurType = SlurType::None;

    AboveBelowType placement = AboveBelowType::None;
    std::string text = "";



    Vec2<float> startPosition = { 0.0f, 0.0f };
    Vec2<float> endPosition = { 0.0f, 0.0f };

    Vec2<float> textPosition = { 0.0f, 0.0f };

    bool isBroken = false; // whether this object is broken across two different systems
};

#endif // MUSIQUE_TABSLUR_H