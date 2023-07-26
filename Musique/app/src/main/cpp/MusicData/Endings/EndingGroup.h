/**
 * This file contains the definition of the 'EndingGroup' class.
 */

#ifndef MUSIQUE_ENDINGGROUP_H
#define MUSIQUE_ENDINGGROUP_H

#include "Ending.h"

class EndingGroup
{
public:
    std::vector<std::shared_ptr<Ending>> endings;
};

#endif //MUSIQUE_ENDINGGROUP_H
