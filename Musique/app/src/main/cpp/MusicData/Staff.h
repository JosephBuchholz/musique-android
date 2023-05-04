#ifndef MUSIQUE_STAFF_H
#define MUSIQUE_STAFF_H

#include "Measures/Measure.h"
#include <vector>

struct StaffTuning {
    Pitch pitch;
};

class Staff {
    friend class Song;
    friend class MusicXMLParser;

public:
    virtual ~Staff() {}


    // gets the max height of the measures from start to end
    float GetMiddleHeight(float lineSpacing) const;
    float GetAboveHeight(float lineSpacing, int start, int end);
    float GetBelowHeight(float lineSpacing, int start, int end);
    float GetTotalHeight(float lineSpacing, int start, int end);

    float GetTotalBeatWidth();

    /**
     * Gets the next (for example if the measure is repeated) beat position of the given measure.
     *
     * @param measureIndex the index of the measure
     * @param currentBeatPosition the current beat position in song
     * @return the next beat position
     */
    float GetMeasureNextBeatPosition(int measureIndex, float currentBeatPosition);

    float GetMeasureBeatPosition(int measureIndex);

protected:
    void CalculateAsPaged(const MusicDisplayConstants& displayConstants);

public:

    enum class StaffType {
        None = 0, Standard, Tab
    };

    StaffType type = StaffType::Standard;

    std::vector<Measure*> measures;

    int lines = 5;

    float maxDistance = 0.0f; // the distance from the top line to the bottom line of the previous staff

    unsigned int capo = 0; // 0 means no capo

    // -- Tab Only --

    std::vector<StaffTuning> tunings;

    TablatureDisplayType tablatureDisplayType = TablatureDisplayType::Full;
};

#endif // MUSIQUE_STAFF_H