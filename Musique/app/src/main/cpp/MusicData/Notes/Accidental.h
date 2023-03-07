#ifndef MUSIQUE_ACCIDENTAL_H
#define MUSIQUE_ACCIDENTAL_H

#include <string>

struct Accidental {

    Accidental() {}

    enum class AccidentalType {
        None = 0, Sharp, Flat, Natural, DoubleSharp, DoubleFlat
    };

    static AccidentalType CalculateAccidentalTypeFromString(const std::string& string) {
        if (string == "sharp") {
            return AccidentalType::Sharp;
        } else if (string == "flat") {
            return AccidentalType::Flat;
        } else if (string == "natural") {
            return AccidentalType::Natural;
        } else if (string == "double-sharp") {
            return AccidentalType::DoubleSharp;
        } else if (string == "flat-flat") {
            return AccidentalType::DoubleFlat;
        }
        return AccidentalType::None;
    }

    AccidentalType accidentalType = AccidentalType::None;

    bool isCautionary = false;
    bool isEditorial = false;

    bool hasBrackets = false;
    bool hasParentheses = false;
};

#endif // MUSIQUE_ACCIDENTAL_H