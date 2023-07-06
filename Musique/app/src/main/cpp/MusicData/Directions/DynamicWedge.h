/**
 * This file contains the definition of the `DynamicWedge` class.
 */

#ifndef MUSIQUE_DYNAMICWEDGE_H
#define MUSIQUE_DYNAMICWEDGE_H

#include "../BaseElements/VisibleElement.h"
#include "../BaseElements/LineElement.h"
#include "../../Collisions/Vec2.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/RenderData.h"
#include "DurationDirection.h"

class DynamicWedgeSegment : public DurationDirectionSegment
{
public:
    void UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX) override;

public:

    bool rightSideIsOpen = false;
    bool leftSideIsOpen = false;
};

/**
 * This class represents a cresc. or dim. wedge/hairpin marking.
 */
class DynamicWedge : public DurationDirection, public LineElement
{
    friend class MusicXMLParser;

public:

    void Render(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const override;
    void RenderDebug(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const override;

    void RenderSegment(RenderData& renderData, std::shared_ptr<DynamicWedgeSegment>, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, Paint paint) const;

    void AddNewSegment(DurationDirectionSegment segment) override;

    int GetSegmentCount() const override { return segments.size(); }
    std::shared_ptr<DurationDirectionSegment> GetSegment(int index) override { return segments[index]; }

    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex, float startMeasurePositionX, float endMeasurePositionX) const override;
    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int segmentIndex, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX) const;

    void UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) override;

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants) override;

private:

    void RenderWedge(RenderData& renderData, Vec2<float> startPosition, Vec2<float> endPosition, bool rightSideIsOpen, bool leftSideIsOpen) const;

public:

    enum class WedgeType
    {
        None = 0, Crescendo, Diminuendo
    } type = WedgeType::None;

    // the height of the wedge
    float spread = 0.0f;

    std::vector<std::shared_ptr<DynamicWedgeSegment>> segments;

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> defaultPositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };

    Vec2<float> relativePositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };

    float defaultSpread = INVALID_FLOAT_VALUE;
};

#endif // MUSIQUE_DYNAMICWEDGE_H
