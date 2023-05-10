/**
 * This file contains the definition of the `AugmentationDot` class.
 */

#ifndef MUSIQUE_AUGMENTATIONDOT_H
#define MUSIQUE_AUGMENTATIONDOT_H

#include "../BaseElements/VisibleElement.h"
#include "../../MusicDisplayConstants.h"

#include "../../RenderData/RenderData.h"

/**
 * Class that represents an augmentation dot that lengthens a note by its duration value * 1.5.
 */
class AugmentationDot : public VisibleElement {
    friend class Note;
    friend class MusicXMLParser;

public:

    /**
     * Renders this class
     *
     * @param[out] renderData The RenderData object to render to
     * @param[in] notePositionX The x position of the parent note
     * @param[in] notePositionY The y position of the parent note
     * @param[in] offsetX offset in x direction
     * @param[in] offsetY offset in y direction
     */
    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const;

protected:

    /**
     * Calculates positioning variables when in paged mode.
     *
     * @param displayConstants display constants for positioning
     * @param noteIsOnLine whether the note that this dot belongs to is on a staff line
     * @param isTab whether the note that this dot belongs to is a tablature note
     */
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, bool noteIsOnLine, bool isTab);

public:
    AboveBelowType placement = AboveBelowType::Above; // whether the dot appears above or below the staff line

    float positionX = 0.0f; // relative to the left hand side of the note
    float positionY = 0.0f; // relative to the top line of the staff
};

#endif // MUSIQUE_AUGMENTATIONDOT_H