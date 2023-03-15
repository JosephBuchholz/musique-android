#ifndef MUSIQUE_TEXTUALELEMENT_H
#define MUSIQUE_TEXTUALELEMENT_H

#include "VisibleElement.h"
#include "Types.h"

/**
 * This class represents a element that is (or has or contains) text.
 */
class TextualElement : public VisibleElement {

public:
    TextualElement() {}

public:
    FontFamily fontFamily = FontFamily();
    FontSize fontSize = FontSize();
    FontStyle fontStyle = FontStyle::Normal;
    FontWeight fontWeight = FontWeight::Normal;

    Justify justify = Justify::Left;

    unsigned int linesThrough = 0;
    unsigned int overline = 0;
    unsigned int underline = 0;
};

#endif // MUSIQUE_TEXTUALELEMENT_H