#ifndef MUSIQUE_REHERSAL_H
#define MUSIQUE_REHERSAL_H

#include "Words.h"

class Rehearsal : public Words {

public:
    Rehearsal() { }

    Rehearsal(Words words)
        : Words(words) { }
};

#endif // MUSIQUE_REHERSAL_H