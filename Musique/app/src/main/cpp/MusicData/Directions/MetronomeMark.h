/**
 * This file contains the definition of the `MetronomeMark` class.
 */

#ifndef MUSIQUE_METRONOMEMARK_H
#define MUSIQUE_METRONOMEMARK_H

#include "../BaseElements/TextualElement.h"
#include "../Types.h"
#include "MetronomeNote.h"
#include "../../MusicDisplayConstants.h"
#include <string>
#include <vector>

/**
 * This class represents a metronome/tempo marking.
 */
class MetronomeMark : public TextualElement
{
    friend class MusicXMLParser;

public:

    /**
     * Renders this class.
     *
     * @param[out] renderData The RenderData object to render to.
     * @param[in] measurePosition The position of the parent measure.
     * @param[in] offset An offset.
     */
    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    Vec2<float> GetDimensions() const;
    Paint GetPaint() const;

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float> &parentPosition);

    void Move(Vec2<float> positionOffset, Vec2<float> sizeOffset = { 0.0f, 0.0f }, float rotationOffset = 0.0f) override;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY);

    static SMuFLID GetMetronomeNoteSMuFLID(NoteValue type);


public:
    NoteUnit mainNoteUnit = NoteUnit();
    NoteUnit mainNoteUnitTied = NoteUnit();
    std::string tempo = "";

    std::vector<MetronomeNote> leftSideNotes;
    std::vector<MetronomeNote> rightSideNotes;

    bool hasParentheses = false;

protected:

    bool noDefX = true;
    bool noDefY = true;
    float defX = 0.0f;
    float defY = 0.0f;

    float relX = 0.0f;
    float relY = 0.0f;
};

#endif // MUSIQUE_METRONOMEMARK_H