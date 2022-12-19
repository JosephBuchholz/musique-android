#pragma once

#include "Renderable.h"
#include "RenderableStaff.h"

#include <vector>

class RenderableInstrument : public Renderable
{
public:
    std::vector<RenderableStaff> staves;
};