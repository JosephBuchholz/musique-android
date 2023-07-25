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

void MidiPlayer::SetVolume(int volume) // from 0 to 100
{
    SetMidiVolume(volume);
}

void MidiPlayer::SetReverb(int reverb)
{
    SetMidiReverb(reverb);
}