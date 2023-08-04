#ifndef MUSIQUE_MIDIPLAYER_H
#define MUSIQUE_MIDIPLAYER_H

#include <array>

#include "Player.h"

#include "../Callbacks.h"
#include "../MusicData/Transpose.h"
#include "../MusicData/Pitch.h"

class MidiPlayer : public Player
{
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

    MidiInstrumentType ToMidiInstrumentType(int value);

    int GetMidiNotePitch(const Pitch& pitch);

    void PlayNote(const PlayableNote& note, int channel) override;
    void StopNote(const PlayableNote& note, int channel) override;

    void PlayUnpitchedNote(const PlayableUnpitchedNote& note) override;
    void StopUnpitchedNote(const PlayableUnpitchedNote& note) override;

    void PitchBendNote(const PlayableNote& note, float alterSemitones, int channel) override;

    void ChangeInstrument(int instrument, int channel = 0) override;

    void StopAllNotes() override;

    void SetVolume(float volume) override;
    void SetReverb(float reverb) override;
    void SetPanoramic(float pan, int channel) override;
    void SetVibrato(float vibrato, int channel) override;
    void SetSustain(bool isOn, int channel) override;

private:
    std::array<int, 16> currentMidiInstruments;
};

#endif // MUSIQUE_MIDIPLAYER_H