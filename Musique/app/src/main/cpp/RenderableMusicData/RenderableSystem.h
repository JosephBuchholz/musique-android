#pragma once

#include "Renderable.h"
#include "RenderableInstrument.h"

#include <vector>

class RenderableSystem : public Renderable
{
public:
    std::vector<RenderableInstrument> instruments;
};