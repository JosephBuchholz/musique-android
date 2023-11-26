#ifndef MUSIQUE_CSMEASURE_H
#define MUSIQUE_CSMEASURE_H

#include <vector>

#include "CSChord.h"
#include "CSLyric.h"

class CSMeasure : public VisibleElement
{
public:

    void Init();

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> parentPosition) const;

    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants) const;

private:

    float GetPositionXFromBeatPositionOfChords(float beatPosition) const;
    std::shared_ptr<CSChord> GetChordFromBeatPosition(float beatPosition);

public:
    float width = 0.0f;

    bool isFirstMeasureOfSystem = false;

    std::vector<std::shared_ptr<CSChord>> chords;
    std::vector<std::shared_ptr<CSLyric>> lyrics;
};

#endif //MUSIQUE_CSMEASURE_H
