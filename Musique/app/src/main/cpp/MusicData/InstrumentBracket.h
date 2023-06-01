/**
 * This file contains the definition of the 'InsturmentBracket' class.
 */

#ifndef MUSIQUE_INSTRUMENTBRACKET_H
#define MUSIQUE_INSTRUMENTBRACKET_H

#include "BaseElements/VisibleElement.h"

/**
 * This class represents the bracket displayed for instrument
 * groups or the brace displayed for piano, harp, or organ parts.
 */
class InstrumentBracket : public VisibleElement
{
public:

    void Render(RenderData& renderData, Vec2<float> parentPosition, float height, Vec2<float> offset = { 0.0f, 0.0f }) const;

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants, float defaultPositionX);

public:

    enum class InstrumentBracketType {
        None = 0, Brace, Bracket
    } type = InstrumentBracketType::Brace;

    // relative to the system/instrument/measure
    float positionX = 0.0f;
};

#endif //MUSIQUE_INSTRUMENTBRACKET_H
