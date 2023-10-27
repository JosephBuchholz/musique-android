/**
 * This file contains the definition of the 'NumberElement' base class.
 */

#ifndef MUSIQUE_NUMBERELEMENT_H
#define MUSIQUE_NUMBERELEMENT_H

#include "TextualElement.h"

/**
 * This class is a base class for number type elements (like page number or measure numbers).
 */
class NumberElement : public TextualElement {

public:

    NumberElement(int number = 0);

    void SetNumber(int number) { this->number = number; displayNumber = ToString(number); };
    int GetNumber() const { return number; }

protected:

    int number;
    std::string displayNumber;

};

#endif //MUSIQUE_NUMBERELEMENT_H
