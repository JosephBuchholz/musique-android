#pragma once

#include "Words.h"

class Rehearsal : public Words {

public:
    Rehearsal() { }

    Rehearsal(Words words)
        : Words(words) { }
};