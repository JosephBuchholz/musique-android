#ifndef MUSIQUE_ENDING_H
#define MUSIQUE_ENDING_H

#include "../BaseElements/TextualElement.h"
#include "../BaseElements/LineElement.h"

class Ending : public TextualElement, public LineElement
{
    friend class Song;
    friend class MusicXMLParser;

public:

    void Render(RenderData& renderData, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

protected:

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:

    std::vector<int> endingNumbers; // the actual ending numbers
    std::string endingNumbersText; // the text that is displayed for the ending numbers

    bool hasFinalDownwardJog = false; // if this ending has a downward jog at the end of the line

    float jogLength = 20.0f; // the length of the downward jog at the start (and end if 'hasFinalDownwardJog') of the line

    int startMeasureIndex; // the measure index at which this ending starts
    int endMeasureIndex; // the measure index at which this ending ends

    bool isBroken = false; // whether the ending is broken across two (or more) different systems.

    // -- Positioning Attributes --

    // relative to the measure position (the x value should always be 0.0f, so as to be centered on the measure)
    Vec2<float> position = { 0.0f, 0.0f };

    // relative to the ending line
    Vec2<float> textPosition = { 0.0f, 0.0f };

protected:

    // -- Positioning Attributes From MusicXML --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> defaultTextPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};


#endif //MUSIQUE_ENDING_H
