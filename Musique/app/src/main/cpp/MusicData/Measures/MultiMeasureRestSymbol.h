/**
 * This file contains the definition of the 'MultiMeasureRestSymbol' class.
 */

#ifndef MUSIQUE_MULTIMEASURERESTSYMBOL_H
#define MUSIQUE_MULTIMEASURERESTSYMBOL_H

#include "../BaseElements/VisibleElement.h"

/**
 * This class represents the symbol and text for multi-measure rests.
 */
class MultiMeasureRestSymbol : public VisibleElement
{

public:

    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants, float lineSpacing, int staffLines, float measureWidth, float measureBeginWidth);

public:

    unsigned int numberOfMeasures = 0;

    bool useSymbols = false;

    // all positions are relative to the parent measure

    // position of the 'bar' symbol
    float startPositionX = 0.0f;
    float endPositionX = 0.0f;

    float positionY = 0.0f;

    // the position of the text
    Vec2<float> textPosition = { 0.0f, 0.0f };
};

#endif //MUSIQUE_MULTIMEASURERESTSYMBOL_H
