#ifndef MUSIQUE_RENDERABLESYSTEM_H
#define MUSIQUE_RENDERABLESYSTEM_H

#include "Renderable.h"
#include "RenderableInstrument.h"

#include <vector>

class RenderableSystem : public Renderable
{
public:
    std::vector<RenderableInstrument> instruments;
};

#endif // MUSIQUE_RENDERABLESYSTEM_H