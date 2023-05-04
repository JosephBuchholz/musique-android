/**
 * This file contains the definition of the `KeySignature` class.
 */

#ifndef MUSIQUE_KEYSIGNATURE_H
#define MUSIQUE_KEYSIGNATURE_H

#include "../../RenderData/RenderData.h"
#include "../Pitch.h"
#include "Clef.h"
#include "../../Collisions/Vec2.h"

/**
 * Represents a musical key signature.
 */
class KeySignature
{
    friend class Measure;

public:
    KeySignature(int fifths) : fifths(fifths) {}

    /**
     * Renders this class
     *
     * @param[out] renderData The `RenderData` object to render to
     * @param[in] showKeySignature Whether the key signature should be shown
     * @param[in] positionX The x position of the key signature
     * @param[in] lineSpacing The space between staff lines
     * @param[in] lines The number of staff lines
     * @param[in] clef The current clef
     * @param[in] offsetX The offset in the x direction
     * @param[in] offsetY The offset in the y direction
     */
    void Render(RenderData& renderData, bool showKeySignature, float positionX, float lineSpacing, int lines, const Clef& clef, float offsetX = 0.0f, float offsetY = 0.0f) const;

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants);

public:
    enum class Mode {
        None = 0, Major, Minor
    };

    static Mode GetModeFromString(const std::string& string) {
        if (string == "major") {
            return Mode::Major;
        } else if (string == "minor") {
            return Mode::Minor;
        }
        return Mode::None;
    }

    int fifths = 0;
    //int previousFifths = 0; // the fifths of the previous key signature
    Mode mode = Mode::None;
    bool print = true;

    bool cancelPreviousKeySignature = false; // whether to cancel (display naturals) the previous key signature
    int previousKeySignatureFifths = 0; // is zero if there was no previous

    // x is relative to the start of the measure
    // y is relative to the top staff line
    Vec2<float> position = { 0.0f, 0.0f };
};

#endif // MUSIQUE_KEYSIGNATURE_H