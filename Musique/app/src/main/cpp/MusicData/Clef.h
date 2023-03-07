#ifndef MUSIQUE_CLEF_H
#define MUSIQUE_CLEF_H

#include <string>

struct Clef {

    Clef() {}

    std::string sign = "G";
    int line = 2;
};

#endif // MUSIQUE_CLEF_H