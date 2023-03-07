#ifndef MUSIQUE_STRING_H
#define MUSIQUE_STRING_H

#include <string>

struct String {
    String() {}

    String(const std::string& string)
        : string(string) {}

    std::string string = "";
    bool print = true;
};

#endif // MUSIQUE_STRING_H