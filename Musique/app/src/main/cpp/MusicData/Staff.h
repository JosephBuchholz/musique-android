#ifndef MUSIQUE_STAFF_H
#define MUSIQUE_STAFF_H

#include "Measures/Measure.h"
#include "BaseElements/VisibleElement.h"

#include <vector>

struct StaffTuning {
    Pitch pitch;
};

class Staff : public VisibleElement
{
    friend class MusicXMLParser;

public:
    virtual ~Staff() {}

    // gets the max height of the measures from start to end
    float GetMiddleHeight(float lineSpacing) const;
    float GetAboveHeight(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex);
    float GetBelowHeight(float lineSpacing, int start, int end);

    float GetTotalHeight(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex) const;

    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex) const;

    float GetTotalBeatWidth();

    float GetLineSpacing(const MusicDisplayConstants& displayConstants) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

    /**
     * Gets the next (for example if the measure is repeated) beat position of the given measure.
     *
     * @param measureIndex the index of the measure
     * @param currentBeatPosition the current beat position in song
     * @return the next beat position
     */
    float GetMeasureNextBeatPosition(int measureIndex, float currentBeatPosition);

    float GetMeasureBeatPosition(int measureIndex);

    void UpdateBoundingBoxes(const MusicDisplayConstants& displayConstants);

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants);

public:

    enum class StaffType {
        None = 0, Standard, Tab
    };

    StaffType type = StaffType::Standard;

    std::vector<std::shared_ptr<Measure>> measures;

    std::vector<std::shared_ptr<DurationDirection>> durationDirections;

    int lines = 5;

    float maxDistance = 0.0f; // the distance from the top line to the bottom line of the previous staff

    unsigned int capo = 0; // 0 means no capo

    std::vector<Vec2<float>> systemPositionData;

    // -- Tab Only --

    std::vector<StaffTuning> tunings;

    TablatureDisplayType tablatureDisplayType = TablatureDisplayType::Full;
};

#endif // MUSIQUE_STAFF_H