#ifndef MUSIQUE_CSLYRIC_H
#define MUSIQUE_CSLYRIC_H

#include "../BaseElements/TextualElement.h"
#include "../../Settings.h"
#include "../BaseElements/TextElement.h"

class CSLyric : TextualElement
{
public:
    void Init(Vec2<float> pos);

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const;

public:

    float beatPosition = 0.0f;
    float duration = 1.0f;

    TextElement lyricText;
};

#endif //MUSIQUE_CSLYRIC_H