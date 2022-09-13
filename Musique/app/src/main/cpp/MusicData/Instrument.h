#pragma once
#include "Staff.h"
#include <vector>
#include <string>
#include "String.h"
#include "MidiInstrument.h"

class Instrument {

public:
    std::string id = "";

    String name = String("Instrument");
    String nameAbbreviation = String();

    MidiInstrument midiInstrument = MidiInstrument();

    std::vector<Staff*> staves;
};