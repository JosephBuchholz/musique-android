/**
 * This file contains the definition of the `Clef` class.
 */

#ifndef MUSIQUE_CLEF_H
#define MUSIQUE_CLEF_H

#include <string>

#include "../../RenderData/RenderData.h"
#include "../../MusicDisplayConstants.h"
#include "../../RenderData/SMuFLID.h"

class RenderData; // TODO: probably temproary??

/**
 * Represents a musical clef.
 */
class Clef {

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

public:
    std::string sign = "";
    int line = 2;
};

#endif // MUSIQUE_CLEF_H