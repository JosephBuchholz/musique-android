/**
 * This file contains the definition of the `BracketDirection` class.
 */

#ifndef MUSIQUE_BRACKETDIRECTION_H
#define MUSIQUE_BRACKETDIRECTION_H

#include "../BaseElements/VisibleElement.h"
#include "../BaseElements/LineElement.h"
#include "../BaseElements/TextElement.h"
#include "../../Collisions/Vec2.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/RenderData.h"
#include "DurationDirection.h"

class BracketDirectionSegment : public DurationDirectionSegment
{
public:
    void Render(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, Paint paint) const;

    void UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX) override;
};

/**
 * This class represents a bracket for part of a direction marking.
 *
 * Note: MusicXML dashes and brackets are represented as this one class.
 */
class BracketDirection : public DurationDirection, public LineElement
{
    friend class MusicXMLParser;

public:

    enum class LineEndType
    {
        None = 0, Up, Down, Both, Arrow, NoEnd
    };

    void Render(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const override;
    void RenderDebug(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const override;

    void AddNewSegment(DurationDirectionSegment segment) override;

    int GetSegmentCount() const override { return segments.size(); }
    std::shared_ptr<DurationDirectionSegment> GetSegment(int index) override { return segments[index]; }

    void Move(Vec2<float> positionOffset, Vec2<float> sizeOffset = { 0.0f, 0.0f }, float rotationOffset = 0.0f) override;

    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex, float startMeasurePositionX, float endMeasurePositionX) const override;
    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int segmentIndex, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX) const;

    void UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) override;

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants) override;

private:

    static void RenderLineEnd(RenderData& renderData, Vec2<float> position, LineEndType lineEndType, float endLength, const Paint& paint = Paint());

public:

    LineEndType lineEndTypeStart = LineEndType::None;
    LineEndType lineEndTypeStop = LineEndType::None;

    // the length of a bracket end
    float endLengthStart = 0.0f;
    float endLengthStop = 0.0f;

    // whether this element is a MusicXML dashes element or bracket element
    bool isDashes = false;

    bool hasText = false;
    TextElement textElement;

    std::vector<std::shared_ptr<BracketDirectionSegment>> segments;

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> defaultPositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };

    Vec2<float> relativePositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif // MUSIQUE_BRACKETDIRECTION_H
