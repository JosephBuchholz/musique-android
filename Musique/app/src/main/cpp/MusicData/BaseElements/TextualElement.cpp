#include "TextualElement.h"

void TextualElement::ModifyPaint(Paint& paint) const
{
    VisibleElement::ModifyPaint(paint);

    paint.textSize = fontSize.size;
    paint.isItalic = (fontStyle == FontStyle::Italic);
    paint.isBold = (fontWeight == FontWeight::Bold);

    switch (align)
    {
        case TextAlign::Left: paint.align = Paint::Align::Left; break;
        case TextAlign::Center: paint.align = Paint::Align::Center; break;
        case TextAlign::Right: paint.align = Paint::Align::Right; break;
        default: break;
    }
}