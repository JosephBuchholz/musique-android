#include "MidiPlayer.h"

MidiPlayer::MidiInstrumentType MidiPlayer::ToMidiInstrumentType(int value)
{
    return MidiInstrumentType(value);
}

int MidiPlayer::GetMidiNotePitch(const Pitch& pitch)
{
    int midiPitch = 12; /*adds 1 one octave since in midi C4 = 60 but (4 * 12 = 48)*/

    midiPitch += (pitch.octave) * 12;
    int num = 0;
    if (pitch.step == DiatonicNote::C) {
        num = 0;
    } else if (pitch.step == DiatonicNote::D) {
        num = 2;
    } else if (pitch.step == DiatonicNote::E) {
        num = 4;
    } else if (pitch.step == DiatonicNote::F) {
        num = 5;
    } else if (pitch.step == DiatonicNote::G) {
        num = 7;
    } else if (pitch.step == DiatonicNote::A) {
        num = 9;
    } else if (pitch.step == DiatonicNote::B) {
        num = 11;
    }
    midiPitch += num;
    midiPitch += pitch.alter;

    return midiPitch;
}

void MidiPlayer::PlayNote(const PlayableNote& note, int channel)
{
    //LOGW("playing pitch: step: %s, alter: %f, octave: %d, freq: %f, channel: %d", note.pitch.step.c_str(), note.pitch.alter, note.pitch.octave, note.pitch.freq, channel);
    //LOGW("playing note: %d, %d, %d, %d", 0x90 | channel, GetMidiNotePitch(note.pitch), 64, channel);
    char event[3];
    event[0] = (char) (0x90 | channel); // message | channel
    event[1] = (char) GetMidiNotePitch(note.pitch); // note

    int velocity = note.velocity;
    if (velocity > 127)
        velocity = 127;
    else if (velocity < 0)
        velocity = 0;

    event[2] = (char) velocity; // velocity
    WriteMidi(event, 3);
}

void MidiPlayer::StopNote(const PlayableNote& note, int channel)
{
    //LOGE("stopping pitch: step: %s, alter: %f, octave: %d, freq: %f, channel: %d", note.pitch.step.c_str(), note.pitch.alter, note.pitch.octave, note.pitch.freq, channel);
    //LOGE("stopping note: %d, %d, %d, %d", 0x80 | channel, GetMidiNotePitch(pitch), 64, channel);
    char event[3];
    event[0] = (char) (0x80 | channel); // message | channel
    event[1] = (char) GetMidiNotePitch(note.pitch); // note
    event[2] = (char) 64; // velocity
    WriteMidi(event, 3);
}

void MidiPlayer::PlayUnpitchedNote(const PlayableUnpitchedNote& note)
{
    char event[3];
    event[0] = (char) (0x90 | 9); // message | channel
    event[1] = (char) note.sound; // note

    int velocity = note.velocity;
    if (velocity > 127)
        velocity = 127;
    else if (velocity < 0)
        velocity = 0;

    event[2] = (char) velocity; // velocity
    WriteMidi(event, 3);
}

void MidiPlayer::StopUnpitchedNote(const PlayableUnpitchedNote& note)
{
    char event[3];
    event[0] = (char) (0x80 | 10); // message | channel
    event[1] = (char) note.sound; // note
    event[2] = (char) 64; // velocity
    WriteMidi(event, 3);
}

#define MIDI_PITCH_BEND_MSG 0xE0

void MidiPlayer::PitchBendNote(const PlayableNote& note, float alterSemitones, int channel)
{
    float semitone = 4096.0f;
    float base = 8192.0f;
    int pitch = int(base + (alterSemitones * semitone));

    char lsb = (char)(pitch & 0x7F); // 1111111
    char msb = (char)((pitch & 0x3F80)>>7); // 11111110000000

    char event[3];
    event[0] = (char) (MIDI_PITCH_BEND_MSG | channel); // message | channel
    event[1] = lsb;
    event[2] = msb;
    WriteMidi(event, 3);
}

void MidiPlayer::ChangeInstrument(int instrument, int channel)
{
    if (currentMidiInstruments[channel] != instrument) {
        char event[2];
        event[0] = (char) (0xC0 | channel); // message | channel
        event[1] = (char) instrument;
        WriteMidi(event, 2);
        currentMidiInstruments[channel] = instrument;
    }
}

#define MIDI_CONTROLLER_MSG 0xB0

#define MIDI_CONTROLLER_MSG_MOD_WHEEL 1
#define MIDI_CONTROLLER_MSG_VOLUME 7
#define MIDI_CONTROLLER_MSG_PAN 10
#define MIDI_CONTROLLER_MSG_EXP 11
#define MIDI_CONTROLLER_MSG_SUSTAIN 64
#define MIDI_CONTROLLER_MSG_CONTROLLERS_OFF 121
#define MIDI_CONTROLLER_MSG_NOTES_OFF 123

#define MIDI_NUM_CHANNELS 16

void MidiPlayer::StopAllNotes()
{
    for (int channel = 0; channel < MIDI_NUM_CHANNELS; channel++)
    {
        char event[3];
        event[0] = (char) (MIDI_CONTROLLER_MSG | channel);
        event[1] = (char) MIDI_CONTROLLER_MSG_NOTES_OFF;
        event[2] = (char) 0;
        WriteMidi(event, 3);
    }
}

void MidiPlayer::SetVolume(float volume)
{
    SetMidiVolume((int)(volume * 100.0f));
}

void MidiPlayer::SetReverb(float reverb)
{
    SetMidiReverb((int)reverb);
}

void MidiPlayer::SetPanoramic(float pan, int channel)
{
    int newPan = int((pan + 1.0f) * (127.0f / 2.0f)); // scale the pan value to 0 - 127 for midi

    char event[3];
    event[0] = (char) (MIDI_CONTROLLER_MSG | channel);
    event[1] = (char) MIDI_CONTROLLER_MSG_PAN;
    event[2] = (char) newPan;
    WriteMidi(event, 3);
}

void MidiPlayer::SetVibrato(float vibrato, int channel)
{
    char event[3];
    event[0] = (char) (MIDI_CONTROLLER_MSG | channel);
    event[1] = (char) MIDI_CONTROLLER_MSG_MOD_WHEEL;
    event[2] = (char) vibrato;
    WriteMidi(event, 3);
}

void MidiPlayer::SetSustain(bool isOn, int channel)
{
    char event[3];
    event[0] = (char) (MIDI_CONTROLLER_MSG | channel);
    event[1] = (char) MIDI_CONTROLLER_MSG_SUSTAIN;
    if (isOn)
        event[2] = (char) 64;
    else
        event[2] = (char) 0;

    WriteMidi(event, 3);
}