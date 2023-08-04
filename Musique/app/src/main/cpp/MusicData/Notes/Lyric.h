/**
 * This file contains the definition of the `Lyric` class.
 */

#ifndef MUSIQUE_LYRIC_H
#define MUSIQUE_LYRIC_H

#include <vector>

#include "../Text.h"
#include "../Types.h"

#include "../BaseElements/TextualElement.h"
#include "../../MusicDisplayConstants.h"

#include "../../RenderData/RenderData.h"

/**
 * This class represents a lyric for a note.
 */
class Lyric : public TextualElement
{
    friend class MusicXMLParser;
    friend class NoteElementParser;

public:
    Lyric() {}

    /**
     * Renders this class
     *
     * @param[out] renderData The RenderData object to render to
     * @param[in] notePositionX The x position of the parent note
     * @param[in] measurePositionY The y position of the parent measure
     */
    void Render(RenderData& renderData, float notePositionX, float measurePositionY) const;
    void RenderDebug(RenderData& renderData, float notePositionX, float measurePositionY) const;

    BoundingBox GetBoundingBox() const;
    BoundingBox GetBoundingBoxRelativeToParent(float notePositionX, float measurePositionY) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(Vec2<float> parentPosition);

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:
    enum class SyllabicType {
        None = 0, Begin, End, Middle, Single
    };

    int number = 1;
    std::vector<LyricText> text;
    std::vector<SyllabicType> syllabics;
    AboveBelowType placement = AboveBelowType::Above;

protected:

    // -- Positioning Attributes From MusicXML --

    float defX = 0.0f;
    float defY = 0.0f;

    float relX = 0.0f;
    float relY = 0.0f;
};

#endif // MUSIQUE_LYRIC_H