#include "Instrument.h"

float Instrument::GetMiddleHeight(float lineSpacing, float tabLineSpacing, int start, int end)
{
    float height = 0.0f;

    /*int i = 0;
    for (auto* staff : staves) {
        float ls = lineSpacing;
        if (staff->type == Staff::StaffType::Tab)
            ls = tabLineSpacing;
        if (i != 0)
            height += staff->GetAboveHeight(ls, start, end);
        height += staff->GetMiddleHeight(ls, start, end);
        if (i != staves.size()-1)
            height += staff->GetBelowHeight(ls, start, end);
        i++;
    }*/

    int i = 0;
    for (auto staff : staves) {
        float ls = lineSpacing;
        if (staff->type == Staff::StaffType::Tab)
            ls = tabLineSpacing;

        height += staff->GetMiddleHeight(ls);

        if (i != 0)
            height += staff->measures[start]->staffDistance;

        i++;
    }

    return height;
}

float Instrument::GetAboveHeight(float lineSpacing, float tabLineSpacing, int start, int end)
{
    //if (staves.size() > 0)
    //    return staves[0]->GetAboveHeight(lineSpacing, start, end);
    return 0.0f;
}

float Instrument::GetBelowHeight(float lineSpacing, float tabLineSpacing, int start, int end)
{
    //if (staves.size() > 0)
    //    return staves[staves.size()-1]->GetBelowHeight(lineSpacing, start, end);
    return 0.0f;
}

float Instrument::GetTotalHeight(float lineSpacing, float tabLineSpacing, int start, int end)
{
    return GetAboveHeight(lineSpacing, tabLineSpacing, start, end) +
            GetMiddleHeight(lineSpacing, tabLineSpacing, start, end) +
            GetBelowHeight(lineSpacing, tabLineSpacing, start, end);
}

BoundingBox Instrument::GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex) const
{
    BoundingBox instrumentBoundingBox;
    bool instBBNotSet = true;

    Vec2<float> previousStaffPosition = { 0.0f, 0.0f };
    float previousStaffBelowAndMiddleHeight = 0.0f;
    bool firstStaff = true;
    for (auto staff : staves)
    {
        Vec2<float> staffPosition = { 0.0f, 0.0f };

        int start = startMeasureIndex;
        int end = endMeasureIndex;

        BoundingBox staffBoundingBox = staff->GetTotalBoundingBox(displayConstants, start, end);
        staffBoundingBox.AddPadding(5.0f);

        if (!firstStaff)
        {
            staffPosition.y = -staffBoundingBox.GetTop();
            staffPosition.y += previousStaffPosition.y;
            staffPosition.y += previousStaffBelowAndMiddleHeight;

            //LOGE("above height: %f, previousStaffY: %f, previousStaffBelowAndMiddleHeight: %f, staffPosition.y: %f", -staffBoundingBox.GetTop(), previousStaffPosition.y, previousStaffBelowAndMiddleHeight, staffPosition.y);
        }

        previousStaffPosition = staffPosition;
        previousStaffBelowAndMiddleHeight = staffBoundingBox.GetBottom();

        staffBoundingBox.position += staffPosition;
        if (instBBNotSet)
            instrumentBoundingBox = staffBoundingBox, instBBNotSet = false;
        else
            instrumentBoundingBox = BoundingBox::CombineBoundingBoxes(instrumentBoundingBox, staffBoundingBox);

        firstStaff = false;
    }

    return instrumentBoundingBox;
}

int Instrument::GetMeasureCount()
{
    if (!staves.empty())
        return staves[0]->measures.size();
    return 0;
}