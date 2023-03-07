#ifndef MUSIQUE_RENDERABLENOTE_H
#define MUSIQUE_RENDERABLENOTE_H

#include "Renderable.h"
#include "../RenderData/Line.h"
#include "../RenderData/RenderBitmap.h"

class RenderableNote : public Renderable
{
public:
    Line* stem;
    RenderBitmap* noteHead;
};

#endif // MUSIQUE_RENDERABLENOTE_H