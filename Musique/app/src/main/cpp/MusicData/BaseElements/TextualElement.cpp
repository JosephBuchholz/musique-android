#include "TextualElement.h"

void TextualElement::ModifyPaint(Paint& paint) const
{
    VisibleElement::ModifyPaint(paint);

    paint.textSize = fontSize.size;
    paint.isItalic = (fontStyle == FontStyle::Italic);
    paint.isBold = (fontWeight == FontWeight::Bold);
}