#pragma once
#include <string>

struct NoteStem {

    NoteStem() {}

    enum class StemType {
        None = 0, Up, Down, Double
    };

    static StemType CalculateStemTypeFromString(const std::string& string) {
        if (string == "up") {
            return StemType::Up;
        } else if (string == "down") {
            return StemType::Down;
        } else if (string == "double") {
            return StemType::Double;
        }
        return StemType::None;
    }

    StemType stemType = StemType::None;
};