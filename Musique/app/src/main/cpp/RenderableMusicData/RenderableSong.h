#ifndef MUSIQUE_RENDERABLESONG_H
#define MUSIQUE_RENDERABLESONG_H

#include "Renderable.h"
#include "RenderableSystem.h"

#include <vector>

class RenderableSong : public Renderable
{
public:
    std::vector<RenderableSystem> systems;
};

#endif // MUSIQUE_RENDERABLESONG_H