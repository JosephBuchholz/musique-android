/**
 * This file contains the definition of the `Slur` class.
 */

#ifndef MUSIQUE_SLUR_H
#define MUSIQUE_SLUR_H

#include "../Types.h"

struct Slur {

    int id = 0;
    StartStopType type = StartStopType::None;
    AboveBelowType placement = AboveBelowType::None;
};

#endif // MUSIQUE_SLUR_H