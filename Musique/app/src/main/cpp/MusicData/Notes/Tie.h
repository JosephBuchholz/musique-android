/**
 * This file contains the definition of the `NoteTie` class.
 */

#ifndef MUSIQUE_TIE_H
#define MUSIQUE_TIE_H

#include "../BaseElements/VisibleElement.h"
#include "Note.h"
#include "../Types.h"

class Note;

/**
 * Class that represents a tie.
 */
class NoteTie : public VisibleElement {
    friend class Note;
    friend class MusicXMLParser;

public:
    enum class TieType
    {
        None = 0, Start, Stop, Continue, LetRing
    } type = TieType::None;

    CurveOrientation orientation = CurveOrientation::Over; // whether the tie is overhand (tips down) or underhand (tips up)

    AboveBelowType placement = AboveBelowType::Above; // whether the tie appears above or below the note

    std::shared_ptr<Note> tiedNote = nullptr; // the note that this tie is tied to (if tie type is start then the note is the second note; if tie type is end then the note is the first note).

    float positionX = 0.0f; // relative to the left hand side of the note
    float positionY = 0.0f; // relative to the top line of the staff
};

#endif // MUSIQUE_TIE_H