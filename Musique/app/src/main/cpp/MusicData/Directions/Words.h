#pragma once

#include "../TextualElement.h"
#include "../Types.h"
#include "../String.h"
#include "../../MusicDisplayConstants.h"

class Words : public TextualElement {
    friend class Song;
    friend class MusicXMLParser;

public:
    Words() {  }

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY);

public:
    String text;

    enum class EnclosureShape {
        None = 0, Rectangle, Square, Oval, Circle, Bracket, InvertedBracket, Triangle, Diamond, Pentagon, Hexagon, Heptagon, Octagon, Nonagon, Decagon
    } enclosureShape = EnclosureShape::None;

    // -- Positioning Attributes --

    float positionX = 0.0f; // relative to measure
    float positionY = 0.0f; // relative to measure

protected:

    bool noDefX = true;
    bool noDefY = true;
    float defX = 0.0f;
    float defY = 0.0f;

    float relX = 0.0f;
    float relY = 0.0f;
};