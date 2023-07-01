/**
 * This file contains the definition of the 'NoteChord' class.
 */

#ifndef MUSIQUE_NOTECHORD_H
#define MUSIQUE_NOTECHORD_H

class NoteChord;
class NoteFlag;

#include "../BaseElements/VisibleElement.h"
#include "../Measures/Measure.h"
#include "Note.h"

/**
 * This class represents a note chord (notes played at the same time (includes intervals))
 */
class NoteChord : public VisibleElement
{
    friend class Song;

public:

    void Render(RenderData& renderData, TablatureDisplayType tabDisplayType, float rootNotePositionYRelativeToMeasure, float topNotePositionYRelativeToMeasure, int lines, Vec2<float> measurePosition, float nextMeasurePositionX, float ls) const;
    void RenderDebug(RenderData& renderData) const;

    /**
     * Get the bounding box containing just the notes.
     *
     * @param displayConstants Display constants.
     * @return The bounding box.
     */
    BoundingBox GetBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const;

    /**
     * Get the bounding box containing everything including the notes,
     * fermatas, and anything else that this object contains.
     *
     * @param displayConstants Display constants.
     * @return The bounding box.
     */
    BoundingBox GetTotalBoundingBoxRelativeToMeasure(const MusicDisplayConstants& displayConstants) const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param displayConstants The display constants.
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition);

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, int staffLines, std::shared_ptr<Measure> measure, bool isMuseScore4);

public:

    std::vector<std::shared_ptr<Note>> notes; // all the notes in the chord (the first one is the root/controlling note)

    std::shared_ptr<NoteStem> noteStem;
    std::shared_ptr<NoteFlag> noteFlag;

    int measureIndex = 0;
};

#endif //MUSIQUE_NOTECHORD_H
