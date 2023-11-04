#ifndef MUSIQUE_CSSTAFF_H
#define MUSIQUE_CSSTAFF_H

#include <vector>

#include "CSMeasure.h"
#include "SyllableGroup.h"

class CSStaff : public VisibleElement
{
public:

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const;

public:

    std::vector<CSMeasure> measures;
    std::vector<SyllableGroup> syllableGroups;
};

#endif //MUSIQUE_CSSTAFF_H
