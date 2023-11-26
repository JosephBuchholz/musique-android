#include "Staff.h"
#include "../Exceptions/Exceptions.h"

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
    //return GetTotalBoundingBox(displayConstants, startMeasureIndex, endMeasureIndex).size.y;
    return 0.0f;
}

BoundingBox Staff::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int systemIndex) const
{
    /*BoundingBox bb;
    bool boundingBoxNotSet = true;

    for (const auto& direction : durationDirections)
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
    }*/

    if (systemIndex < systemBoundingBoxes.size())
    {
        return systemBoundingBoxes[systemIndex];
    }
    return {};
}

void Staff::CalculateTotalBoundingBoxes(const MusicDisplayConstants& displayConstants, const std::vector<std::shared_ptr<System>>& systems)
{
    for (const auto& system : systems)
    {
        BoundingBox bb;
        bool boundingBoxNotSet = true;

        for (const auto& direction : durationDirections)
        {
            BoundingBox directionBoundingBox = direction->GetTotalBoundingBox(displayConstants, system->beginningMeasureIndex, system->endingMeasureIndex, 0.0f, 0.0f);

            if (boundingBoxNotSet)
                bb = directionBoundingBox, boundingBoxNotSet = false;
            else
                bb = BoundingBox::CombineBoundingBoxes(bb, directionBoundingBox);
        }

        for (int i = system->beginningMeasureIndex; i <= system->endingMeasureIndex; i++)
        {
            BoundingBox measureBoundingBox;

            if (type == StaffType::ChordSheet)
            {
                if (i >= csStaff->measures.size())
                {
                    LOGE_TAG("Staff", "Index is bigger than measures.size()!!");
                    break;
                }

                measureBoundingBox = csStaff->measures[i]->GetTotalBoundingBox(displayConstants);
            }
            else
            {
                if (i >= measures.size())
                {
                    LOGE_TAG("Staff", "Index is bigger than measures.size()!!");
                    break;
                }

                measureBoundingBox = measures[i]->GetTotalBoundingBox(displayConstants, lines);
            }

            if (boundingBoxNotSet)
                bb = measureBoundingBox, boundingBoxNotSet = false;
            else
                bb = BoundingBox::CombineBoundingBoxes(bb, measureBoundingBox);
        }

        systemBoundingBoxes.push_back(bb);
    }
}

float Staff::GetTotalBeatWidth()
{
    float width = 0.0f;

    for (const auto& m : measures) {
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
        for (const auto& m : measures) {

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
    for (const auto& m : measures) {

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

std::pair<int, float> Staff::GetMeasureFromSoundBeatPosition(float beatPosition, const std::vector<std::shared_ptr<EndingGroup>>& endingGroups)
{
    std::unordered_map<int, int> repeatCounts;

    int measureIndex = 0;
    float measureBeatPosition = 0.0f;

    std::unordered_map<int, int> endingCounts;

    for (int i = 0; i < endingGroups.size(); i++)
        endingCounts[i] = 1;

    for (int mi = 0; mi < measures.size(); mi++)
    {
        std::shared_ptr<Ending> ending = nullptr;
        std::shared_ptr<EndingGroup> endingGroup = nullptr;
        int endingGroupIndex = 0;
        for (const auto& eg : endingGroups)
        {
            for (const auto& e : eg->endings)
            {
                if (e->startMeasureIndex == mi)
                {
                    ending = e;
                    endingGroup = eg;
                    break;
                }
            }

            if (ending)
                break;

            endingGroupIndex++;
        }

        if (ending && endingGroup) // measure is at an ending
        {
            bool foundCorrectEnding = false;
            for (const auto& e : endingGroup->endings)
            {
                for (int number : e->endingNumbers)
                {
                    if (endingCounts.find(endingGroupIndex) != endingCounts.end()) // found
                    {
                        if (endingCounts[endingGroupIndex] == number) // found ending
                        {
                            foundCorrectEnding = true;
                            endingCounts[endingGroupIndex]++;
                            mi = e->startMeasureIndex;
                            break;
                        }
                    }
                    else
                    {
                        LOGE_TAG("Staff.cpp", "not found");
                    }
                }

                if (foundCorrectEnding)
                    break;
            }

            if (!foundCorrectEnding)
                LOGE_TAG("Staff.cpp", "was note able to find correct ending for playback");
        } // end of ending code

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

float Staff::GetSoundBeatPositionFromVisualBeatPosition(float beatPosition, const std::vector<std::shared_ptr<EndingGroup>>& endingGroups)
{
    float soundBeatPosition = 0.0f;

    for (const auto& measure : measures)
    {
        if (beatPosition >= measure->beatPosition && beatPosition < measure->beatPosition + measure->duration.duration)
        {
            std::unordered_map<int, int> repeatCounts;
            float measureBeatPosition = 0.0f;

            for (int mi = 0; mi < measures.size(); mi++)
            {
                if (mi == measure->index)
                {
                    soundBeatPosition = measureBeatPosition;
                    soundBeatPosition += beatPosition - measure->beatPosition;
                    return soundBeatPosition;
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
        }
    }

    return soundBeatPosition;
}

void Staff::Transpose(const TranspositionRequest& transposeRequest)
{
    for (auto measure : measures)
    {
        measure->OnTranspose(transposeRequest);
    }
}

float Staff::GetPositionY(int systemIndex) const
{
    ASSERT(systemIndex < systemPositionData.size());
    ASSERT(systemIndex >= 0);

    return systemPositionData[systemIndex].y;
}
