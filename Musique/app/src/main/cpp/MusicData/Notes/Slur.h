#pragma once
#include "../Types.h"

struct Slur {

    int id = 0;
    StartStopType type = StartStopType::None;
    AboveBelowType placement = AboveBelowType::None;
};