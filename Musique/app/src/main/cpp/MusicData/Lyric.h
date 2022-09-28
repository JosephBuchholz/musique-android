#pragma once
#include <vector>

#include "Text.h"
#include "Types.h"

#include "TextualElement.h"

class Lyric : public TextualElement {

public:
    Lyric() {}

public:
    enum class SyllabicType {
        None = 0, Begin, End, Middle, Single
    };

    int number = 1;
    std::vector<LyricText> text;
    std::vector<SyllabicType> syllabics;
    AboveBelowType placement = AboveBelowType::Above;
};