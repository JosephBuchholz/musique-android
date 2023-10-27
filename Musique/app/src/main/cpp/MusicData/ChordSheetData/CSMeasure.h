#ifndef MUSIQUE_CSMEASURE_H
#define MUSIQUE_CSMEASURE_H

#include <vector>

#include "CSChord.h"

class CSMeasure : public VisibleElement
{
public:

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const;

public:
    float width = 0.0f;

    std::vector<CSChord> chords;
};

#endif //MUSIQUE_CSMEASURE_H
