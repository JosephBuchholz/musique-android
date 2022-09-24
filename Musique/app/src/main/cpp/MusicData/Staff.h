#pragma once
#include "Measure.h"
#include <vector>

struct StaffTuning {
    Pitch pitch;
};

class Staff {

public:
    virtual ~Staff() {}

    // gets the max height of the measures from start to end
    float GetMiddleHeight(float lineSpacing, int start, int end);
    float GetAboveHeight(float lineSpacing, int start, int end);
    float GetBelowHeight(float lineSpacing, int start, int end);
    float GetTotalHeight(float lineSpacing, int start, int end);

    // gets the next(for example if the measure is repeated) beat position of the given measure
    float GetMeasureNextBeatPosition(int measureIndex, float currentBeatPosition) {
        float position = 0.0f;

        Measure* measure = measures[measureIndex];

        if (measure->nextBeatPosition == -1 || measure->nextBeatPosition < currentBeatPosition) { // calculate new beat position
            int i = 0;
            for (auto* m : measures) {

                if (i == measureIndex) {
                    break;
                } else {
                    position += m->GetDuration();
                }

                i++;
            }

            measure->nextBeatPosition = position;
        } else { // use old beat position
            position = measure->nextBeatPosition;
        }

        return position;
    }

    float GetMeasureBeatPosition(int measureIndex) {
        float position = 0.0f;

        int i = 0;
        for (auto* m : measures) {

            if (i == measureIndex) {
                break;
            } else {
                position += m->GetDuration();
            }

            i++;
        }

        return position;
    }

    enum class StaffType {
        None = 0, Standard, Tab
    };

    StaffType type = StaffType::Standard;

    std::vector<Measure*> measures;

    int lines = 5;

    // -- Tab Only --

    std::vector<StaffTuning> tunings;
};