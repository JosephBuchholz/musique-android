#pragma once

#include "Renderable.h"
#include "RenderableNote.h"
#include "../RenderData/Line.h"

#include <vector>
#include <memory>

class RenderableMeasure : public Renderable
{
public:
    std::vector<RenderableNote> notes;

    std::vector<std::shared_ptr<Line>> staffLines;
};