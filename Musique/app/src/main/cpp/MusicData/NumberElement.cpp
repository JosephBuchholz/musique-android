#include "NumberElement.h"

NumberElement::NumberElement(int number)
        : number(number)
{
    displayNumber = ToString(number);
}