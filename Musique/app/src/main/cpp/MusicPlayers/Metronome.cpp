#include "Metronome.h"

void Metronome::OnUpdate(std::shared_ptr<Player> player, const TimeSignature& timeSignature, float measureBeatPosition, float playLineBeatPosition, float previousPlayLineBeatPosition)
{
    float beat = 4.0f / (float) timeSignature.noteType;
    for (int i = 0; i < timeSignature.notes; i++)
    {
        float metronomeBeatPosition = ((float) i * beat) + measureBeatPosition;
        PlayableUnpitchedNote::NoteSound metronomeSound = PlayableUnpitchedNote::NoteSound::MetronomeClick;
        if (i == 0)
            metronomeSound = PlayableUnpitchedNote::NoteSound::MetronomeBell;

        if (playLineBeatPosition >= metronomeBeatPosition && previousPlayLineBeatPosition <= metronomeBeatPosition)
        {
            PlayableUnpitchedNote note;
            note.sound = metronomeSound;
            player->PlayUnpitchedNote(note);
        }
    }
}