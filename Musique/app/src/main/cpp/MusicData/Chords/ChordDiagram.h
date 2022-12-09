#pragma once

#include <vector>
#include "../TextualElement.h"
#include "../BaseElement.h"
#include "../Types.h"

class ChordDiagramNote : public BaseElement {

public:

    StringValue string;
    FretValue fret;
    unsigned int fingering = 0;
    StartStopType barre = StartStopType::None;
};

class ChordDiagram : public TextualElement {

public:
    ChordDiagram() {}

public:

    StringValue string;
    FretValue frets;

    FretValue firstFret = 1;
    RightLeftType firstFretLocation = RightLeftType::Left;

    std::vector<ChordDiagramNote> notes;
};