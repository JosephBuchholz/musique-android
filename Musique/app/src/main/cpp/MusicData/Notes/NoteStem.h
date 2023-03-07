#ifndef MUSIQUE_NOTESTEM_H
#define MUSIQUE_NOTESTEM_H

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

    // relative to left side of note (or center of note for tablature)
    float stemPositionX = 0.0f;

    // relative to the center of the note
    float stemStartY = 0.0f; // closest to the note
    float stemEndY = 0.0f; // farthest away from the note
};

#endif // MUSIQUE_NOTESTEM_H