#ifndef MUSIQUE_RENDERABLEINSTRUMENT_H
#define MUSIQUE_RENDERABLEINSTRUMENT_H

#include "Renderable.h"
#include "RenderableStaff.h"

#include <vector>

class RenderableInstrument : public Renderable
{
public:
    std::vector<RenderableStaff> staves;
};

#endif // MUSIQUE_RENDERABLEINSTRUMENT_H