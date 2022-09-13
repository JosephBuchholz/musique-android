#pragma once
#include <string>

struct String {
    String() {}

    String(const std::string& string)
        : string(string) {}

    std::string string = "";
    bool print = true;
};