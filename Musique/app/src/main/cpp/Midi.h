#ifndef MUSIQUE_MIDI_H
#define MUSIQUE_MIDI_H

#include "Callbacks.h"
#include "MusicData/Transpose.h"

class Midi {

public:
    enum class MidiInstrumentType : uint8_t {
        AcousticGrandPiano = 0,
        BrightAcousticPiano = 1,
        ElectricPiano1 = 4,
        Glockenspiel = 9,
        Vibraphone = 11,
        Marimba = 12,
        Xylophone = 13,
        ChurchOrgan = 19,
        Harmonica = 22,
        AcousticGuitarNylon = 24,
        AcousticGuitarSteel = 25,
        ElectricGuitarJazz = 26,
        ElectricGuitarClean = 27,
        ElectricGuitarMuted = 28,
        OverdrivenGuitar = 29,
        DistortionGuitar = 30,
        GuitarHarmonics = 31,
        AcousticBass = 32,
        ElectricBassFingered = 33,
        Violin = 40,
        Viola = 41,
        Cello = 42,
        Trumpet = 56,
        SopranoSax = 64,
        Flute = 73,
        Recorder = 74,
        Lead1 = 80,
        Pad1 = 88,
        Sitar = 104
    };

    MidiInstrumentType ToMidiInstrumentType(int value)
    {
        return MidiInstrumentType(value);
    }

    int GetMidiNotePitch(Pitch pitch, Transpose transpose)
    {
        int midiPitch = 12;

        midiPitch += (pitch.octave + transpose.octaveChange) * 12;
        int num = 0;
        if (pitch.step == "C") {
            num = 0;
        } else if (pitch.step == "D") {
            num = 2;
        } else if (pitch.step == "E") {
            num = 4;
        } else if (pitch.step == "F") {
            num = 5;
        } else if (pitch.step == "G") {
            num = 7;
        } else if (pitch.step == "A") {
            num = 9;
        } else if (pitch.step == "B") {
            num = 11;
        }
        midiPitch += num;
        midiPitch += pitch.alter;

        return midiPitch;
    }

    void PlayNote(std::shared_ptr<Note> note, std::shared_ptr<Measure> measure, int channel = 0)
    {
        if (!note->isRest)
        {
            char event[3];
            event[0] = (char)(0x90 | channel); // message | channel
            event[1] = (char)GetMidiNotePitch(note->pitch, measure->transpose); // note
            event[2] = (char)64; // velocity
            WriteMidi(event, 3);
        }
    }

    void StopNote(std::shared_ptr<Note> note, std::shared_ptr<Measure> measure, int channel = 0)
    {
        if (!note->isRest)
        {
            char event[3];
            event[0] = (char) (0x80 | channel); // message | channel
            event[1] = (char) GetMidiNotePitch(note->pitch, measure->transpose); // note
            event[2] = (char) 64; // velocity
            WriteMidi(event, 3);
        }
    }

    void ChangeInstrument(int instrument, int channel = 0) {
        if (currentMidiInstruments[channel] != instrument) {
            char event[2];
            event[0] = (char) (0xC0 | channel); // message | channel
            event[1] = (char) instrument;
            WriteMidi(event, 2);
            currentMidiInstruments[channel] = instrument;
        }
    }

    void SetVolume(int volume) // from 0 to 100
    {
        SetMidiVolume(volume);
    }

    void SetReverb(int reverb)
    {
        SetMidiReverb(reverb);
    }

private:
    std::array<int, 16> currentMidiInstruments;
};

#endif // MUSIQUE_MIDI_H