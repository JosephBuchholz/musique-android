#include "AugmentationDot.h"

#define AUGMENTATION_DOT_X_OFFSET 3.0f

void AugmentationDot::Render(RenderData& renderData, Vec2<float> notePosition) const
{
    Paint paint;
    VisibleElement::ModifyPaint(paint);

    renderData.AddGlyph(SMuFLGlyph(SMuFLID::augmentationDot, position + notePosition, paint));
}

void AugmentationDot::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, bool noteIsOnLine, bool isTab, float noteWidth)
{
    position = { 0.0f, 0.0f };

    if (isTab)
    {
        if (placement == AboveBelowType::Above)
            position.y -= displayConstants.tabLineSpacing * 0.5f;
        else if (placement == AboveBelowType::Below)
            position.y += displayConstants.tabLineSpacing * 0.5f;

        position.x += noteWidth;
        position.x += AUGMENTATION_DOT_X_OFFSET;
    }
    else
    {
        if (noteIsOnLine) // if the dot is on a staff line
        {
            if (placement == AboveBelowType::Above)
                position.y -= displayConstants.lineSpacing * 0.5f;
            else if (placement == AboveBelowType::Below)
                position.y += displayConstants.lineSpacing * 0.5f;
        }

        position.x += noteWidth;
        position.x += AUGMENTATION_DOT_X_OFFSET;
    }
}