#ifndef MUSIQUE_MIDIINSTRUMENT_H
#define MUSIQUE_MIDIINSTRUMENT_H

#include <string>

class MidiInstrument {

public:
    std::string id = "";

    int channel = 1;
    int program = 1;
    int volume = 80;
    int pan = 0;
};

#endif // MUSIQUE_MIDIINSTRUMENT_H