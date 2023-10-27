#ifndef MUSIQUE_CSSTAFF_H
#define MUSIQUE_CSSTAFF_H

#include <vector>

#include "CSMeasure.h"

class CSStaff : public VisibleElement
{
public:

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const;

public:

    std::vector<CSMeasure> measures;
};

#endif //MUSIQUE_CSSTAFF_H
