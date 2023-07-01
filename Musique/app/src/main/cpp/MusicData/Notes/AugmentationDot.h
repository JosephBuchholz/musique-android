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
class AugmentationDot : public VisibleElement
{
    friend class MusicXMLParser;

public:

    /**
     * Renders this class.
     *
     * @param[out] renderData The RenderData object to render to.
     * @param[in] notePosition The position of the parent note.
     */
    void Render(RenderData& renderData, Vec2<float> notePosition) const;

    /**
     * Calculates positioning variables when in paged mode.
     *
     * @param displayConstants Display constants for positioning.
     * @param noteIsOnLine Whether the note that this dot belongs to is on a staff line.
     * @param isTab Whether the note that this dot belongs to is a tablature note.
     * @param noteWidth The width of the parent note.
     */
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, bool noteIsOnLine, bool isTab, float noteWidth);

public:
    AboveBelowType placement = AboveBelowType::Above; // whether the dot appears above or below the staff line
};

#endif // MUSIQUE_AUGMENTATIONDOT_H