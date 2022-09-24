#include "Staff.h"

float Staff::GetMiddleHeight(float lineSpacing, int start, int end)
{
    float maxHeight = 0.0f;
    for (int i = start; i <= end; i++) {
        float height = measures[i]->GetMiddleHeight(lines, lineSpacing);
        if (height > maxHeight)
            maxHeight = height;
    }
    return maxHeight;
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
                  GetMiddleHeight(lineSpacing, start, end) +
                  GetBelowHeight(lineSpacing, start, end);
}