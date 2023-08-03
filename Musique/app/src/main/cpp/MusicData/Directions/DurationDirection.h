/**
 * This file contains the definitions of the 'DurationDirection' and 'DurationDirectionSegment' classes.
 */

#ifndef MUSIQUE_DURATIONDIRECTION_H
#define MUSIQUE_DURATIONDIRECTION_H

#include "../BaseElements/VisibleElement.h"

class DurationDirectionSegment : public VisibleElement
{
public:

    virtual void Move(Vec2<float> positionOffset, Vec2<float> sizeOffset = { 0.0f, 0.0f }, float rotationOffset = 0.0f) override;

    virtual void UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX) {};

public:

    int startMeasureIndex = 0; // the measure that this object starts in
    int endMeasureIndex = 0; // the measure that this object ends in

    Vec2<float> positionStart = { 0.0f, 0.0f };
    Vec2<float> positionEnd = { 0.0f, 0.0f };
};

/**
 * This class is a base class for a musical direction that lasts
 * for some duration (for multiple beats or measures). Examples include:
 * crescendo or diminuendo hairpin markings; or various brackets.
 */
class DurationDirection : public VisibleElement
{
public:

    /**
     * Renders this object.
     *
     * @param[in,out] renderData The 'RenderData' to render to.
     * @param[in] staffPositionY The y position of the staff.
     * @param[in] startMeasurePositionX The x position of the start measure given by the 'startMeasureIndex' parameter.
     * @param[in] endMeasurePositionX The x position of the end measure given by the 'endMeasureIndex' parameter.
     * @param[in] startMeasureIndex The measure index of the measure to start on.
     * @param[in] endMeasureIndex The measure index of the measure to end on.
     */
    virtual void Render(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const = 0;

    /**
     * Same as the 'Render' function except for debug rendering.
     */
    virtual void RenderDebug(RenderData& renderData, float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) const = 0;

    /**
     * Adds a new segment.
     *
     * @param segment The segment to add.
     */
    virtual void AddNewSegment(DurationDirectionSegment segment) = 0;

    /**
     * Calculates the positioning attributes of this class when the display mode is Paged.
     *
     * @param displayConstants The constants for rendering.
     */
    virtual void CalculateAsPaged(const MusicDisplayConstants& displayConstants) = 0;

    virtual int GetSegmentCount() const = 0;
    virtual std::shared_ptr<DurationDirectionSegment> GetSegment(int index) = 0;

    virtual void Move(Vec2<float> positionOffset, Vec2<float> sizeOffset = { 0.0f, 0.0f }, float rotationOffset = 0.0f) override {};

    virtual BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int startMeasureIndex, int endMeasureIndex, float startMeasurePositionX, float endMeasurePositionX) const = 0;

    virtual void UpdateBoundingBox(float staffPositionY, float startMeasurePositionX, float endMeasurePositionX, int startMeasureIndex, int endMeasureIndex) = 0;

public:
    // starting beat position
    float beatPositionStart = 0.0f; // the position in the measure in beats(quarter notes)
    float beatPositionInSongStart = 0.0f; // the position in the song(not counting repeats) in beats(quarter notes)

    // ending beat position
    float beatPositionEnd = 0.0f;
    float beatPositionInSongEnd = 0.0f;

    int startMeasureIndex = 0; // the measure that this object starts in
    int endMeasureIndex = 0; // the measure that this object ends in

    bool isBroken = false; // whether this object is split across two or more systems
};

#endif //MUSIQUE_DURATIONDIRECTION_H
