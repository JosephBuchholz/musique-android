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