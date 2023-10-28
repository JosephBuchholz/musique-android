#ifndef MUSIQUE_STAFF_H
#define MUSIQUE_STAFF_H

#include "Measures/Measure.h"
#include "BaseElements/VisibleElement.h"
#include "Endings/Ending.h"
#include "Endings/EndingGroup.h"
#include "../TranspositionRequest.h"
#include "ChordSheetData/CSStaff.h"

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

    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int systemIndex) const;

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

    void CalculateTotalBoundingBoxes(const MusicDisplayConstants& displayConstants, const std::vector<std::shared_ptr<System>>& systems);

    /**
     * Gets the y position of this staff in the system of the given index. Throws
     * an InvalidArgumentException if the system index is invalid.
     *
     * @param systemIndex The index of the system.
     * @return The y position.
     */
    float GetPositionY(int systemIndex) const;

    /* ----- Sound Related Functions ----- */

    std::pair<int, float> GetMeasureFromSoundBeatPosition(float beatPosition, const std::vector<std::shared_ptr<EndingGroup>>& endingGroups);
    float GetSoundBeatPositionFromVisualBeatPosition(float beatPosition, const std::vector<std::shared_ptr<EndingGroup>>& endingGroups);

    void Transpose(const TranspositionRequest& transposeRequest);

public:

    enum class StaffType {
        None = 0, Standard, Tab, ChordSheet
    };

    StaffType type = StaffType::Standard;

    std::vector<std::shared_ptr<Measure>> measures;

    std::vector<std::shared_ptr<DurationDirection>> durationDirections;

    std::shared_ptr<CSStaff> csStaff;

    int lines = 5;

    float maxDistance = 0.0f; // the distance from the top line to the bottom line of the previous staff

    unsigned int capo = 0; // 0 means no capo

    std::vector<Vec2<float>> systemPositionData;
    std::vector<BoundingBox> systemBoundingBoxes;

    // -- Tab Only --

    std::vector<StaffTuning> tunings;

    TablatureDisplayType tablatureDisplayType = TablatureDisplayType::Full;



    float currentVelocity = 64.0f;
};

#endif // MUSIQUE_STAFF_H