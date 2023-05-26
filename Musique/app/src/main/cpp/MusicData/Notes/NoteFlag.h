/**
 * This class contains the definition of the 'NoteFlag' class.
 */

#ifndef MUSIQUE_NOTEFLAG_H
#define MUSIQUE_NOTEFLAG_H

#include "../BaseElements/VisibleElement.h"

/**
 * This class represents the flag on a note.
 */
class NoteFlag : public VisibleElement
{
    friend class Note;
    friend class NoteChord;

public:

    void Render(RenderData& renderData, Vec2<float> endOfNoteStemPosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    SMuFLID GetSMuFLID() const;

protected:

    void CalculateAsPaged(const MusicDisplayConstants& displayConstants, float defaultSize);

public:

    enum class Type
    {
        None = 0, Up, Down
    } type = Type::Up;

    NoteValue noteDuration = NoteValue::None;

    // the size of this object (as a percentage)
    float size = 1.0f;

    // relative to the end of the note stem
    Vec2<float> position = { 0.0f, 0.0f };
};

#endif //MUSIQUE_NOTEFLAG_H
