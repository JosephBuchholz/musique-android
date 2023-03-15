#include "LineElement.h"

void LineElement::ModifyPaint(Paint& paint) const
{
    if (lineType == LineType::Dashed)
    {
        paint.isDashedLine = true;
        paint.dashLength = dashLength;
        paint.dashSpaceLength = dashSpaceLength;
    }
    else if (lineType == LineType::Dotted)
    {
        paint.isDottedLine = true;
        paint.dashSpaceLength = dashSpaceLength;
        paint.dotRadius = 1.5f;
    }
}