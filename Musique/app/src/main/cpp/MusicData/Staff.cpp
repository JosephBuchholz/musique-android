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

float Staff::GetAboveHeight(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex)
{
    float maxHeight = 0.0f;
    for (int i = startMeasureIndex; i <= endMeasureIndex; i++)
    {
        if (i >= measures.size())
        {
            LOGE("Index is bigger than measures.size()!!");
            break;
        }
        maxHeight = std::max(maxHeight, measures[i]->GetAboveHeight(displayConstants, lines));
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

float Staff::GetTotalHeight(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex) const
{
    return GetTotalBoundingBox(displayConstants, startMeasureIndex, endMeasureIndex).size.y;
}

BoundingBox Staff::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex) const
{
    BoundingBox bb;
    bool boundingBoxNotSet = true;

    for (auto direction : durationDirections)
    {
        BoundingBox directionBoundingBox = direction->GetTotalBoundingBox(displayConstants, startMeasureIndex, endMeasureIndex, 0.0f, 0.0f);

        if (boundingBoxNotSet)
            bb = directionBoundingBox, boundingBoxNotSet = false;
        else
            bb = BoundingBox::CombineBoundingBoxes(bb, directionBoundingBox);
    }

    for (int i = startMeasureIndex; i <= endMeasureIndex; i++)
    {
        if (i >= measures.size())
        {
            LOGE("Index is bigger than measures.size()!!");
            break;
        }

        BoundingBox measureBoundingBox = measures[i]->GetTotalBoundingBox(displayConstants, lines);

        if (boundingBoxNotSet)
            bb = measureBoundingBox, boundingBoxNotSet = false;
        else
            bb = BoundingBox::CombineBoundingBoxes(bb, measureBoundingBox);
    }

    return bb;
}

float Staff::GetTotalBeatWidth()
{
    float width = 0.0f;

    for (auto m : measures) {
        width += m->GetDuration();
    }

    return width;
}

float Staff::GetLineSpacing(const MusicDisplayConstants& displayConstants) const
{
    float lineSpacing = displayConstants.lineSpacing;
    if (type == Staff::StaffType::Tab)
        lineSpacing = displayConstants.tabLineSpacing;

    return lineSpacing;
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

void Staff::RenderDebug(RenderData& renderData) const
{
    for (auto measure : measures)
    {
        //measure->RenderDebug(renderData);
    }

    boundingBox.Render(renderData, (int)0x66FF0000);
}

void Staff::UpdateBoundingBoxes(const MusicDisplayConstants& displayConstants)
{
    //boundingBox.size.x = 100.0f;
    //boundingBox.size.y = GetTotalHeight(displayConstants);
}

std::pair<int, float> Staff::GetMeasureFromSoundBeatPosition(float beatPosition)
{
    std::unordered_map<int, int> repeatCounts;

    int measureIndex = 0;
    float measureBeatPosition = 0.0f;

    for (int mi = 0; mi < measures.size(); mi++)
    {
        if (beatPosition >= measureBeatPosition && beatPosition <= measureBeatPosition + measures[mi]->duration.duration)
        {
            return std::make_pair(mi, measureBeatPosition);
        }

        measureBeatPosition += measures[mi]->duration.duration;

        if (measures[mi]->IsRepeatBackward())
        {
            if (repeatCounts.find(mi) != repeatCounts.end()) // found
            {
                repeatCounts[mi]++;
            }
            else
            {
                repeatCounts[mi] = 1;
            }

            if (repeatCounts[mi] < measures[mi]->GetRepeatCount())
            {
                bool repeatIsAtBeginning = true;
                for (int mj = mi; mj > 0; mj--)
                {
                    if (measures[mj]->IsRepeatForward())
                    {
                        mi = mj - 1;
                        repeatIsAtBeginning = false;
                        break;
                    }
                }

                if (repeatIsAtBeginning)
                    mi = -1;
            }
        }
    }

    return std::make_pair(0, 0.0f);
}