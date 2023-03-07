#ifndef MUSIQUE_AUGMENTATIONDOT_H
#define MUSIQUE_AUGMENTATIONDOT_H

#include "../VisibleElement.h"
#include "../../MusicDisplayConstants.h"

/**
 * Class that represents an augmentation dot that lengthens a note by its duration value * 1.5.
 */
class AugmentationDot : public VisibleElement {
    friend class Note;
    friend class MusicXMLParser;

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