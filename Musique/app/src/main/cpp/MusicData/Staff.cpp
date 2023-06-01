#include "Staff.h"

float Staff::GetMiddleHeight(float lineSpacing) const
{
    /*float maxHeight = 0.0f;
    for (int i = start; i <= end; i++) {
        float height = measures[i]->GetMiddleHeight(lines, lineSpacing);
        if (height > maxHeight)
            maxHeight = height;
    }*/
    return (lineSpacing * float(lines - 1));
}

float Staff::GetAboveHeight(float lineSpacing, int start, int end)
{
    float maxHeight = 0.0f;
    for (int i = start; i <= end; i++) {
        float height = measures[i]->GetAboveHeight(lines, lineSpacing);
        if (height > maxHeight)
            maxHeight = height;
    }
    return maxHeight;
}

float Staff::GetBelowHeight(float lineSpacing, int start, int end)
{
    float maxHeight = 0.0f;
    for (int i = start; i <= end; i++) {
        float height = measures[i]->GetBelowHeight(lines, lineSpacing);
        if (height > maxHeight)
            maxHeight = height;
    }
    return maxHeight;
}

float Staff::GetTotalHeight(float lineSpacing, int start, int end)
{
    return GetAboveHeight(lineSpacing, start, end) +
                  GetMiddleHeight(lineSpacing) +
                  GetBelowHeight(lineSpacing, start, end);
}

float Staff::GetTotalBeatWidth()
{
    float width = 0.0f;

    for (auto m : measures) {
        width += m->GetDuration();
    }

    return width;
}

float Staff::GetMeasureNextBeatPosition(int measureIndex, float currentBeatPosition) {
    float position = 0.0f;

    std::shared_ptr<Measure> measure = measures[measureIndex];

    if (measure->nextBeatPosition == -1 || measure->nextBeatPosition < currentBeatPosition) { // calculate new beat position
        int i = 0;
        for (auto m : measures) {

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

float Staff::GetMeasureBeatPosition(int measureIndex) {
    float position = 0.0f;

    int i = 0;
    for (auto m : measures) {

        if (!m)
        {
            LOGE("measure is nullptr!!!");
            break;
        }

        if (i == measureIndex) {
            break;
        } else {
            position += m->GetDuration();
        }

        i++;
    }

    return position;
}

void Staff::CalculateAsPaged(const MusicDisplayConstants& displayConstants)
{

}