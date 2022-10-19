#pragma once

#include "TextualElement.h"
#include "Types.h"
#include "MetronomeNote.h"
#include <string>
#include <vector>

class MetronomeMark : public TextualElement {

public:
    MetronomeMark() {  }

public:
    NoteUnit mainNoteUnit = NoteUnit();
    NoteUnit mainNoteUnitTied = NoteUnit();
    std::string tempo = "";

    std::vector<MetronomeNote> leftSideNotes;
    std::vector<MetronomeNote> rightSideNotes;
};