/**
 * This file contains things for debugging (such as logging)
 */

#ifndef MUSIQUE_DEBUG_H
#define MUSIQUE_DEBUG_H

#define DEBUG 1

#include "Logging.h"

#if DEBUG

#define SHOW_BOUNDING_BOXES 0

#if SHOW_BOUNDING_BOXES
    #define DEBUG_BOUNDING_BOXES 1
#endif

#endif

#endif //MUSIQUE_DEBUG_H
