#pragma once

#include "Renderable.h"
#include "RenderableSystem.h"

#include <vector>

class RenderableSong : public Renderable
{
public:
    std::vector<RenderableSystem> systems;
};