/**
 * This file contains the definition of the `TABSlur` class.
 */

#ifndef MUSIQUE_TABSLUR_H
#define MUSIQUE_TABSLUR_H

#include "../Types.h"
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

#endif // MUSIQUE_TABSLUR_H