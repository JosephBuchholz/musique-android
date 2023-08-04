#ifndef MUSIQUE_PLAYER_H
#define MUSIQUE_PLAYER_H

#include "../MusicData/Pitch.h"

class PlayableNote
{
public:
    Pitch pitch;
    int velocity = 64;
};

class PlayableUnpitchedNote
{
public:

    enum class NoteSound
    {
        HighQ = 27,
        Slap = 28,
        ScratchPush = 29,
        ScratchPull = 30,
        Sticks = 31,
        SquareClick = 32,
        MetronomeClick = 33,
        MetronomeBell = 34,
        BassDrum2 = 35,
        BassDrum1 = 36,
        SideStick = 37,
        SnareDrum1 = 38,
        HandClap = 39,
        SnareDrum2 = 40,
        LowTom2 = 41,
        ClosedHiHat = 42,
        LowTom1 = 43,
        PedalHiHat = 44,
        MidTom2 = 45,
        OpenHiHat = 46,
        MidTom1 = 47,
        HighTom2 = 48,
        CrashCymbal1 = 49,
        HighTom1 = 50,
        RideCymbal1 = 51,
        ChineseCymbal = 52,
        RideBell = 53,
        Tambourine = 54,
        SplashCymbal = 55,
        Cowbell = 56,
        CrashCymbal2 = 57,
        VibraSlap = 58,
        RideCymbal2 = 59,
        HighBongo = 60,
        LowBongo = 61,
        MuteHighConga = 62,
        OpenHighConga = 63,
        LowConga = 64,
        HighTimbale = 65,
        LowTimbale = 66,
        HighAgogo = 67,
        LowAgogo = 68,
        Cabasa = 69,
        Maracas = 70,
        ShortWhistle = 71,
        LongWhistle = 72,
        ShortGuiro = 73,
        LongGuiro = 74,
        Claves = 75,
        HighWoodBlock = 76,
        LowWoodBlock = 77,
        MuteCuica = 78,
        OpenCuica = 79,
        MuteTriangle = 80,
        OpenTriangle = 81,
        Shaker = 82,
        JingleBell = 83,
        Belltree = 84,
        Castanets = 85,
        MuteSurdo = 86,
        OpenSurdo = 87,
    } sound = NoteSound::BassDrum2;

    int velocity = 64;
};

class Player
{
public:

    enum class InstrumentSound : uint8_t
    {
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

public:

    virtual void PlayNote(const PlayableNote& note, int channel = 0) = 0;
    virtual void StopNote(const PlayableNote& note, int channel = 0) = 0;

    virtual void PlayUnpitchedNote(const PlayableUnpitchedNote& note) = 0;
    virtual void StopUnpitchedNote(const PlayableUnpitchedNote& note) = 0;

    virtual void PitchBendNote(const PlayableNote& note, float alterSemitones, int channel = 0) = 0;

    virtual void ChangeInstrument(int instrument, int channel = 0) = 0;

    virtual void StopAllNotes() = 0;

    virtual void SetVolume(float volume) = 0;
    virtual void SetReverb(float reverb) = 0;

    /**
     * Sets the pan on the specified channel, -1.0 is fully left, 0.0 is the center, and 1.0 is fully right.
     *
     * @param pan The pan value (from -1.0 to 1.0).
     * @param channel The channel to apply the pan to.
     */
    virtual void SetPanoramic(float pan, int channel) = 0;

    virtual void SetVibrato(float vibrato, int channel) = 0;
    virtual void SetSustain(bool isOn, int channel) = 0;
};

#endif //MUSIQUE_PLAYER_H
