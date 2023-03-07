#ifndef MUSIQUE_BASEELEMENT_H
#define MUSIQUE_BASEELEMENT_H

#include <string>

// the base class of all notation elements
class BaseElement {

public:
    BaseElement() {}

public:
    std::string id = "";
};

#endif // MUSIQUE_BASEELEMENT_H