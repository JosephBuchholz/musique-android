/**
 * This file contains the definition of the `Rehearsal` class.
 */

#ifndef MUSIQUE_REHERSAL_H
#define MUSIQUE_REHERSAL_H

#include "Words.h"

/**
 * This class represents a rehearsal mark.
 */
class Rehearsal : public Words {

public:
    Rehearsal() { }

    Rehearsal(Words words)
        : Words(words) { }
};

#endif // MUSIQUE_REHERSAL_H