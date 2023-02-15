#pragma once

#include "../BaseElement.h"

#include "Rehearsal.h"
#include "Words.h"
#include "MetronomeMark.h"

#include "../Types.h"

#include <vector>

#include <memory>

class Direction : public BaseElement {
public:
    Direction() {}

public:

    float beatPosition = 0.0f; // the position of the note in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the note in the song(not counting repeats) in beats(quarter notes)

    std::vector<Rehearsal> rehearsals;
    std::vector<Words> words;
    std::shared_ptr<MetronomeMark> metronomeMark = nullptr;

    AboveBelowType placement = AboveBelowType::Above;
};
