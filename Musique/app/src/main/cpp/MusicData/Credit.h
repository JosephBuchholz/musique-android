#ifndef MUSIQUE_CREDIT_H
#define MUSIQUE_CREDIT_H

#include "TextualElement.h"

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