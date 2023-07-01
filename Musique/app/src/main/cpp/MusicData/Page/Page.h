#ifndef MUSIQUE_PAGE_H
#define MUSIQUE_PAGE_H

#include <vector>

#include "../../Collisions/Vec2.h"
#include "../../MusicDisplayConstants.h"
#include "PageNumber.h"

class Page
{
public:

    Vec2<float> pageDimensions = { DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT };

    float leftMargin = DEFAULT_LEFT_MARGIN;
    float rightMargin = DEFAULT_RIGHT_MARGIN;
    float topMargin = DEFAULT_TOP_MARGIN;
    float bottomMargin = DEFAULT_BOTTOM_MARGIN;

    PageNumber pageNumber;

    std::vector<std::shared_ptr<System>> systems;
};

#endif //MUSIQUE_PAGE_H
