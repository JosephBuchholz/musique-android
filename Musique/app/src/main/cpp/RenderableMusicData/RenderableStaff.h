#pragma once

#include "Renderable.h"
#include "RenderableMeasure.h"

#include <vector>

class RenderableStaff : public Renderable
{
public:
    std::vector<RenderableMeasure> measures;
    std::vector<Line*> measureLines;
};