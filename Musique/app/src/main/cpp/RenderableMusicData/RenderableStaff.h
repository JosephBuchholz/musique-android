#ifndef MUSIQUE_RENDERABLESTAFF_H
#define MUSIQUE_RENDERABLESTAFF_H

#include "Renderable.h"
#include "RenderableMeasure.h"

#include <vector>

class RenderableStaff : public Renderable
{
public:
    std::vector<RenderableMeasure> measures;
    std::vector<Line*> measureLines;
};

#endif // MUSIQUE_RENDERABLESTAFF_H