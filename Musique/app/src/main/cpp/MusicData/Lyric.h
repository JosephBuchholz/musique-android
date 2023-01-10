#pragma once
#include <vector>

#include "Text.h"
#include "Types.h"

#include "TextualElement.h"
#include "../MusicDisplayConstants.h"

class Lyric : public TextualElement {
    friend class Song;
    friend class MusicXMLParser;

public:
    Lyric() {}

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:
    enum class SyllabicType {
        None = 0, Begin, End, Middle, Single
    };

    float positionX = 0.0f;
    float positionY = 0.0f;

    int number = 1;
    std::vector<LyricText> text;
    std::vector<SyllabicType> syllabics;
    AboveBelowType placement = AboveBelowType::Above;

protected:

    // -- Positioning Attributes From MusicXML --

    float defX = 0.0f;
    float defY = 0.0f;

    float relX = 0.0f;
    float relY = 0.0f;
};