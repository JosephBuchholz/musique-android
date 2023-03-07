#ifndef MUSIQUE_PAINTRENDERDATA_H
#define MUSIQUE_PAINTRENDERDATA_H

#include <vector>

#include "RenderData.h"

/**
 * An object that holds a list of pages to be printed on paper.
 */
struct PrintRenderData {

public:
    std::vector<RenderData> pages;
};

#endif // MUSIQUE_PAINTRENDERDATA_H