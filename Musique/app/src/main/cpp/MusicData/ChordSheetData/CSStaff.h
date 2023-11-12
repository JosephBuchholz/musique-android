#ifndef MUSIQUE_CSSTAFF_H
#define MUSIQUE_CSSTAFF_H

#include <vector>

#include "CSMeasure.h"
#include "SyllableGroup.h"
#include "LyricalPhrase.h"

class CSStaff : public VisibleElement
{
public:

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const;

public:

    std::vector<std::shared_ptr<CSMeasure>> measures;
    std::vector<std::shared_ptr<SyllableGroup>> syllableGroups;
    std::vector<std::shared_ptr<LyricalPhrase>> lyricalPhrases;
};

#endif //MUSIQUE_CSSTAFF_H
