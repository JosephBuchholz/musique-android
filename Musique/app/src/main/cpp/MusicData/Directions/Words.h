/**
 * This file contains the definition of the `Words` class.
 */

#ifndef MUSIQUE_WORDS_H
#define MUSIQUE_WORDS_H

#include "../BaseElements/TextualElement.h"
#include "../Types.h"
#include "../String.h"
#include "../../MusicDisplayConstants.h"
#include "../../Collisions/Vec2.h"
#include "../SoundEvent.h"
#include "../../TranspositionRequest.h"

/**
 * This class represents a textual music direction (such as rit. or cresc.).
 */
class Words : public TextualElement
{
    friend class MusicXMLParser;

public:
    Words() {  }

    /**
     * Renders this class.
     *
     * @param[out] renderData The RenderData object to render to.
     * @param[in] measurePosition The position of the parent measure.
     */
    virtual void Render(RenderData& renderData, Vec2<float> measurePosition) const;
    virtual void RenderDebug(RenderData& renderData, Vec2<float> measurePosition) const;

    BoundingBox GetBoundingBoxRelativeToParent() const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float> &parentPosition);

    Vec2<float> GetDimensions() const;

    void Move(Vec2<float> positionOffset, Vec2<float> sizeOffset = { 0.0f, 0.0f }, float rotationOffset = 0.0f) override;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY);

    std::shared_ptr<SoundEvent> GetSoundEvent() const;

    void OnTranspose(const TranspositionRequest& transposeRequest);

public:
    String text;

    enum class EnclosureShape {
        None = 0, Rectangle, Square, Oval, Circle, Bracket, InvertedBracket, Triangle, Diamond, Pentagon, Hexagon, Heptagon, Octagon, Nonagon, Decagon
    } enclosureShape = EnclosureShape::None;

protected:

    bool noDefX = true;
    bool noDefY = true;
    float defX = 0.0f;
    float defY = 0.0f;

    float relX = 0.0f;
    float relY = 0.0f;
};

#endif // MUSIQUE_WORDS_H