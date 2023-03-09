#ifndef MUSIQUE_LYRIC_H
#define MUSIQUE_LYRIC_H

#include <vector>

#include "../Text.h"
#include "../Types.h"

#include "../TextualElement.h"
#include "../../MusicDisplayConstants.h"

#include "../../RenderData/RenderData.h"

class Lyric : public TextualElement {
    friend class Song;
    friend class MusicXMLParser;

public:
    Lyric() {}

    /**
     * Renders this class
     *
     * @param[out] renderData The RenderData object to render to
     * @param[in] notePositionX The x position of the parent note
     * @param[in] measurePositionY The y position of the parent measure
     * @param[in] offsetX offset in x direction
     * @param[in] offsetY offset in y direction
     */
    void Render(RenderData& renderData, float notePositionX, float measurePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const;

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:
    enum class SyllabicType {
        None = 0, Begin, End, Middle, Single
    };

    float positionX = 0.0f;
    float positionY = 0.0f;

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