#ifndef MUSIQUE_CREDIT_H
#define MUSIQUE_CREDIT_H

#include "../BaseElements/TextualElement.h"

#include "../../Collisions/Vec2.h"

class CreditWords : public TextualElement
{
    friend class MusicXMLParser;
    friend class Credit;

public:

    std::string text;

    float positionX;
    float positionY;

protected:
    float defaultX;
    float defaultY;
};

class Credit : public BaseElement {

public:

    /**
     * Renders this class.
     *
     * @param renderData The render data to render to.
     * @param pagePosition The position of the current page.
     * @param pageDimensions The dimensions of the current page.
     * @param offset An offset.
     */
    void Render(RenderData& renderData, Vec2<float> pagePosition, Vec2<float> pageDimensions, Vec2<float> offset = { 0.0f, 0.0f }) const;

public:
    void CalculatePositionAsPaged()
    {
        words.positionX = words.defaultX;
        words.positionY = words.defaultY;
    }

public:
    unsigned int pageNumber = 1;

    enum class CreditType {
        None = 0, PageNumber, Title, Subtitle, Composer, Arranger, Lyricist, Rights, PartName, Transcriber
    } creditType = CreditType::None;

    CreditWords words;
};

#endif // MUSIQUE_CREDIT_H