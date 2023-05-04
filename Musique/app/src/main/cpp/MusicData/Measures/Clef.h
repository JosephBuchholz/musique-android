/**
 * This file contains the definition of the `Clef` class.
 */

#ifndef MUSIQUE_CLEF_H
#define MUSIQUE_CLEF_H

#include <string>

#include "../../RenderData/RenderData.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/SMuFLID.h"
#include "../../Collisions/Vec2.h"

class RenderData; // TODO: probably temproary??

/**
 * Represents a musical clef.
 */
class Clef
{
    friend class Measure;

public:
    Clef() {}

    /**
     * Renders this class.
     *
     * @param[out] renderData The `RenderData` object to render to.
     * @param[in] showClef Whether the clef should be shown.
     * @param[in] positionX The x position of the key signature.
     * @param[in] displayConstants Constants for rendering.
     * @param[in] lines The number of staff lines.
     * @param[in] offsetX The offset in the x direction.
     * @param[in] offsetY The offset in the y direction.
     */
    void Render(RenderData& renderData, bool showClef, float positionX, const MusicDisplayConstants& displayConstants, int lines, float offsetX = 0.0f, float offsetY = 0.0f) const;

    /**
     * Gets the y position of the staff line that this clef sits on (relative to the bottom staff line).
     *
     * @param displayConstants Constants for rendering.
     * @param lines The number of staff lines.
     * @return The y position of the clef line (relative to the bottom staff line)
     */
    float GetClefLinePositionY(const MusicDisplayConstants& displayConstants, int lines) const;

    static SMuFLID GetClefSMuFLID(const Clef& clef, int staffLines);

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines);

public:

    bool operator==(const Clef& rhs) const
    {
        return (this->sign == rhs.sign) && (this->line == rhs.line) && (this->octaveChange == rhs.octaveChange);
    }

    bool operator!=(const Clef& rhs) const
    {
        return !(*this == rhs);
    }

public:
    std::string sign = "";
    int line = 2;
    int octaveChange = 0; // the change in octave for the clef

    bool clefChanged = false; // if this is a clef for a clef change

    // x is relative to the start of the measure
    // y is relative to the top staff line
    Vec2<float> position = { 0.0f, 0.0f };
};

#endif // MUSIQUE_CLEF_H