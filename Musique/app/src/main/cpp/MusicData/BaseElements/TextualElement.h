#ifndef MUSIQUE_TEXTUALELEMENT_H
#define MUSIQUE_TEXTUALELEMENT_H

#include "VisibleElement.h"
#include "../Types.h"

class VisibleElement;

/**
 * This class represents a element that is (or has or contains) text.
 */
class TextualElement : public VisibleElement
{

public:
    TextualElement() {}

    /**
     * Updates the given paint to match the attributes of this class.
     *
     * @param[out] paint The paint object to be modified.
     */
    void ModifyPaint(Paint& paint) const;

public:
    FontFamily fontFamily = FontFamily();
    FontSize fontSize = FontSize();
    FontStyle fontStyle = FontStyle::Normal;
    FontWeight fontWeight = FontWeight::Normal;

    Justify justify = Justify::Left;

    enum class TextAlign {
        None = 0, Left, Center, Right
    } align = TextAlign::Center;

    unsigned int linesThrough = 0;
    unsigned int overline = 0;
    unsigned int underline = 0;
};

#endif // MUSIQUE_TEXTUALELEMENT_H