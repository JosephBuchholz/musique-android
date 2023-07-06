/**
 *
 */

#ifndef MUSIQUE_TEXTELEMENT_H
#define MUSIQUE_TEXTELEMENT_H

#include "TextualElement.h"

#include <string>

class TextElement : public TextualElement
{
public:

    void Render(RenderData& renderData, Vec2<float> parentPosition, Paint parentPaint) const;

    BoundingBox GetBoundingBox(Paint parentPaint) const;
    Vec2<float> GetDimensions(Paint parentPaint) const;

public:

    std::string text = "";
};

#endif //MUSIQUE_TEXTELEMENT_H
