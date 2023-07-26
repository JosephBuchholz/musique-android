#ifndef MUSIQUE_ENDING_H
#define MUSIQUE_ENDING_H

#include "../BaseElements/TextualElement.h"
#include "../BaseElements/LineElement.h"

class EndingSegment
{
public:

    unsigned int startMeasureIndex = 0; // the measure that this object starts in
    unsigned int endMeasureIndex = 0; // the measure that this object ends in

    Vec2<float> positionStart = { 0.0f, 0.0f };
    Vec2<float> positionEnd = { 0.0f, 0.0f };

    bool isStart = false;
    bool isEnd = false;
};

class Ending : public TextualElement, public LineElement
{
    friend class MusicXMLParser;

public:

    void Render(RenderData& renderData, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, int startIndex, int endIndex) const;

    void RenderSegment(RenderData& renderData, std::shared_ptr<EndingSegment> segment, Vec2<float> measureStartPosition, Vec2<float> measureEndPosition, Paint paint) const;

    BoundingBox GetBoundingBoxRelativeToParent() const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(Vec2<float> parentPosition);

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:

    std::vector<int> endingNumbers; // the actual ending numbers
    std::string endingNumbersText; // the text that is displayed for the ending numbers

    bool hasFinalDownwardJog = false; // if this ending has a downward jog at the end of the line

    float jogLength = 20.0f; // the length of the downward jog at the start (and end if 'hasFinalDownwardJog') of the line

    int startMeasureIndex; // the measure index at which this ending starts
    int endMeasureIndex; // the measure index at which this ending ends

    bool isBroken = false; // whether the ending is broken across two (or more) different systems.

    bool isLastEndingInGroup = false;

    std::vector<std::shared_ptr<EndingSegment>> segments;

    // -- Positioning Attributes --

    // relative to the ending line
    Vec2<float> textPosition = { 0.0f, 0.0f };

protected:

    // -- Positioning Attributes From MusicXML --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> defaultTextPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};


#endif //MUSIQUE_ENDING_H
