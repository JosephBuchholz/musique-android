#include "Measure.h"

float Measure::GetMiddleHeight(float staffLineCount, float lineSpacing)
{
    return (lineSpacing * (staffLineCount - 1));
}

float Measure::GetAboveHeight(float staffLineCount, float lineSpacing)
{
    return 50.0f;
}

float Measure::GetBelowHeight(float staffLineCount, float lineSpacing)
{
    return 50.0f;
}

float Measure::GetTotalHeight(float staffLineCount, float lineSpacing)
{
    return GetAboveHeight(staffLineCount, lineSpacing) +
    GetMiddleHeight(staffLineCount, lineSpacing) +
    GetBelowHeight(staffLineCount, lineSpacing);
}