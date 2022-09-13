#pragma once
#include "Types.h"
#include <string>

struct TABSlur {

    enum class SlurType {
        None = 0, HammerOn, PullOff
    };

    int id = 0;
    SlurType slurType = SlurType::None;
    StartStopType type = StartStopType::None;
    AboveBelowType placement = AboveBelowType::None;
    std::string text = "";
};