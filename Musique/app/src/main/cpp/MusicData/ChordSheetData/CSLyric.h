#ifndef MUSIQUE_CSLYRIC_H
#define MUSIQUE_CSLYRIC_H

#include "../BaseElements/TextualElement.h"
#include "../../Settings.h"
#include "../BaseElements/TextElement.h"
#include "SyllableGroup.h"
#include "LyricalPhrase.h"

class CSLyric : public TextualElement
{
public:
    void Init(Vec2<float> pos);

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const;

public:

    float beatPosition = 0.0f;
    float duration = 1.0f;

    TextElement lyricText;

    std::shared_ptr<SyllableGroup> parentSyllableGroup;
    std::shared_ptr<LyricalPhrase> parentLyricalPhrase;

    bool isPickupToNextMeasure = false;
};

#endif //MUSIQUE_CSLYRIC_H