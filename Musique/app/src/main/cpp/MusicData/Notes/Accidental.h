/**
 * This file contains the definition of the `Accidental` class.
 */

#ifndef MUSIQUE_ACCIDENTAL_H
#define MUSIQUE_ACCIDENTAL_H

#include <string>

#include "../BaseElements/VisibleElement.h"
#include "../../RenderData/RenderData.h"

/**
 * This class represents a musical accidental for a note.
 */
class Accidental : public VisibleElement
{
public:
    enum class AccidentalType {
        None = 0, Sharp, Flat, Natural, DoubleSharp, DoubleFlat
    };

public:

    void Render(RenderData& renderData, Vec2<float> parentPosition) const;

    Vec2<float> GetDimensions() const;

    SMuFLID GetSMuFLID() const;
    Paint GetPaint() const;

    /**
     * Converts a string to (like "sharp") and converts it
     * to the `AccidentalType` enum (like AccidentalType::Sharp)
     *
     * @param string The string to convert
     * @return The value that was converted
     */
    static AccidentalType CalculateAccidentalTypeFromString(const std::string& string);

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants, NoteSize noteSize);

public:

    AccidentalType accidentalType = AccidentalType::None;

    bool isCautionary = false;
    bool isEditorial = false;

    bool hasBrackets = false;
    bool hasParentheses = false;

    // the size of this object (as a percentage)
    float size = 1.0f;

    Vec2<float> position = { 0.0f, 0.0f };
};

#endif // MUSIQUE_ACCIDENTAL_H