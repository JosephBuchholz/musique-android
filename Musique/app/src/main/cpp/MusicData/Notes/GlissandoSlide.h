/**
 * This file contains the definition of the 'GlissandoSlide' class.
 */

#ifndef MUSIQUE_GLISSANDOSLIDE_H
#define MUSIQUE_GLISSANDOSLIDE_H

#include "../BaseElements/TextualElement.h"
#include "../BaseElements/LineElement.h"

#include "../ComplexLine.h"

class GlissandoSlide;

#include "Note.h"

/**
 * This class represents both glissandos and slides.
 */
class GlissandoSlide : public TextualElement, public LineElement
{
    friend class NoteElementParser;

public:

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPositionCenter The center position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float>& parentPositionCenter);

    void Render(RenderData& renderData, Vec2<float> startNotePosition, Vec2<float> endNotePosition, bool isFirstNote, Vec2<float> offset = { 0.0f, 0.0f }) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defPositionStart, Vec2<float> defPositionEnd, bool isTab);

private:
    void RenderLine(RenderData& renderData, Vec2<float> startPosition, Vec2<float> endPosition) const;

public:

    std::pair<std::shared_ptr<Note>, std::shared_ptr<Note>> notes; // the notes associated with this gliss/slide

    // whether this is a glissando or a slide
    enum class Type {
        None = 0, Glissando, Slide
    } type = Type::Glissando;

    std::string text = "";
    bool displayText = false;

    bool isSlideUp = true; // whether the gliss/slide goes up or down in pitch (it is true if it goes up in pitch)

    std::shared_ptr<ComplexLine> complexLine = nullptr;

    // -- Positioning Attributes --

    // relative to the starting note
    Vec2<float> positionStart = { 0.0f, 0.0f };

    // relative to the ending note
    Vec2<float> positionEnd = { 0.0f, 0.0f };

    bool isBroken = false; // whether this gliss/slide is broken across two systems (starts in one system and ends in another system)

protected:

    // -- Positioning Attributes From MusicXML --

    Vec2<float> defaultPositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> defaultPositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };

    Vec2<float> relativePositionStart = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePositionEnd = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif //MUSIQUE_GLISSANDOSLIDE_H
