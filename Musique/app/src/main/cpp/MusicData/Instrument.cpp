#include "Instrument.h"

float Instrument::GetMiddleHeight(float lineSpacing, float tabLineSpacing, int start, int end)
{
    float height = 0.0f;
    int i = 0;
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
    }
    return height;
}

float Instrument::GetAboveHeight(float lineSpacing, float tabLineSpacing, int start, int end)
{
    if (staves.size() > 0)
        return staves[0]->GetAboveHeight(lineSpacing, start, end);
    return 0.0f;
}

float Instrument::GetBelowHeight(float lineSpacing, float tabLineSpacing, int start, int end)
{
    if (staves.size() > 0)
        return staves[staves.size()-1]->GetBelowHeight(lineSpacing, start, end);
    return 0.0f;
}

float Instrument::GetTotalHeight(float lineSpacing, float tabLineSpacing, int start, int end)
{
    return GetAboveHeight(lineSpacing, tabLineSpacing, start, end) +
            GetMiddleHeight(lineSpacing, tabLineSpacing, start, end) +
            GetBelowHeight(lineSpacing, tabLineSpacing, start, end);
}

int Instrument::GetMeasureCount()
{
    if (staves.size() > 0)
        return staves[0]->measures.size();
    return 0;
}