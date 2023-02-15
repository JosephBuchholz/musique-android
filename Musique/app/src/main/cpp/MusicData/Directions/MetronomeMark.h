#pragma once

#include "../TextualElement.h"
#include "../Types.h"
#include "MetronomeNote.h"
#include "../../MusicDisplayConstants.h"
#include <string>
#include <vector>

class MetronomeMark : public TextualElement {
    friend class Song;
    friend class MusicXMLParser;

public:
    MetronomeMark() {  }

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY);

public:
    NoteUnit mainNoteUnit = NoteUnit();
    NoteUnit mainNoteUnitTied = NoteUnit();
    std::string tempo = "";

    std::vector<MetronomeNote> leftSideNotes;
    std::vector<MetronomeNote> rightSideNotes;

    bool hasParentheses = false;

    // -- Positioning Attributes --

    float positionX = 0.0f; // relative to measure
    float positionY = 0.0f; // relative to measure

protected:

    bool noDefX = true;
    bool noDefY = true;
    float defX = 0.0f;
    float defY = 0.0f;

    float relX = 0.0f;
    float relY = 0.0f;
};