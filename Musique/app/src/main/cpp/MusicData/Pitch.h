#pragma once
#include <string>

struct Pitch {

    Pitch() {}

    std::string step = "C";
    int octave = 4;
    float alter = 0.0f;
    float freq = 0.0f;
};