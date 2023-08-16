#ifndef MUSIQUE_CHORDDIAGRAM_H
#define MUSIQUE_CHORDDIAGRAM_H

#include <vector>
#include "../BaseElements/TextualElement.h"
#include "../BaseElements/BaseElement.h"
#include "../Types.h"

class ChordDiagramNote : public BaseElement {

public:

    StringValue string;
    FretValue fret;
    unsigned int fingering = 0;
    StartStopType barre = StartStopType::None;
};


/**
 * This class represents a guitar chord diagram.
 */
class ChordDiagram : public TextualElement {

    friend class Chord;

public:

    /**
     * Renders this object.
     *
     * @param renderData The render data to render to.
     * @param chordPosition The position of the parent chord.
     */
    void Render(RenderData& renderData, Vec2<float> chordPosition = { 0.0f, 0.0f }) const;

    BoundingBox GetBoundingBoxRelativeToParent() const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float>& parentPosition);

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPosition);

public:

    StringValue strings;
    FretValue frets;

    FretValue firstFret = 1;
    RightLeftType firstFretLocation = RightLeftType::Left;

    std::vector<ChordDiagramNote> notes;

    float width = 35.0f;
    float height = 40.0f;

    // position is relative to the position of the chord (the parent)
    Vec2<float> position;
};

#endif // MUSIQUE_CHORDDIAGRAM_H