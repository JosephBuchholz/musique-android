#pragma once

#include "../BaseElement.h"

#include "Rehearsal.h"
#include "Words.h"
#include "../MetronomeMark.h"

#include "../Types.h"

#include "../../../../../../../../../../../AppData/Local/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/vector"

class Direction : public BaseElement {
public:
    Direction() {}

public:

    float beatPosition = 0.0f; // the position of the note in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the note in the song(not counting repeats) in beats(quarter notes)

    std::vector<Rehearsal> rehearsals;
    std::vector<Words> words;
    MetronomeMark* metronomeMark = nullptr;

    AboveBelowType placement = AboveBelowType::Above;
};
