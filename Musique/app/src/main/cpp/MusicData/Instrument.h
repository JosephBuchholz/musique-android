#pragma once
#include "Staff.h"
#include <vector>
#include <string>
#include "String.h"
#include "MidiInstrument.h"

class Instrument {

public:
    // gets the max height of the stave's measures from start to end
    float GetMiddleHeight(float lineSpacing, float tabLineSpacing, int start, int end);
    float GetAboveHeight(float lineSpacing, float tabLineSpacing, int start, int end);
    float GetBelowHeight(float lineSpacing, float tabLineSpacing, int start, int end);
    float GetTotalHeight(float lineSpacing, float tabLineSpacing, int start, int end);

    // gets the number of measures
    int GetMeasureCount();

public:
    std::string id = "";

    String name = String("Instrument");
    String nameAbbreviation = String();

    MidiInstrument midiInstrument = MidiInstrument();

    std::vector<Staff*> staves;
};