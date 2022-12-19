#pragma once

#include "Renderable.h"
#include "../RenderData/Line.h"
#include "../RenderData/RenderBitmap.h"

class RenderableNote : public Renderable
{
public:
    Line* stem;
    RenderBitmap* noteHead;
};