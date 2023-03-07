#ifndef MUSIQUE_RENDERABLEMEASURE_H
#define MUSIQUE_RENDERABLEMEASURE_H

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

#endif // MUSIQUE_RENDERABLEMEASURE_H