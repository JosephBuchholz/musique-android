/**
 * This file contains the definition of the `Accidental` class.
 */

#ifndef MUSIQUE_ACCIDENTAL_H
#define MUSIQUE_ACCIDENTAL_H

#include <string>

/**
 * This class represents a musical accidental for a note.
 */
class Accidental
{
public:
    Accidental() {}

    enum class AccidentalType {
        None = 0, Sharp, Flat, Natural, DoubleSharp, DoubleFlat
    };

    /**
     * Converts a string to (like "sharp") and converts it
     * to the `AccidentalType` enum (like AccidentalType::Sharp)
     *
     * @param string The string to convert
     * @return The value that was converted
     */
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