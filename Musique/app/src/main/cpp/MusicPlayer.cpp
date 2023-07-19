#include "MusicPlayer.h"

#include "MidiPlayer.h"

MusicPlayer::MusicPlayer()
{
    player = std::make_shared<MidiPlayer>();
    metronome = std::make_shared<Metronome>();
}

void MusicPlayer::OnUpdate(double dt, std::shared_ptr<Song> song)
{
    double dts = dt / 1000.0; // delta time in seconds
    double dtm = dts / 60.0; // delta time in minutes

    if (playing)
    {
        float previousPlayLineBeatPosition = playLineBeatPosition;
        playLineBeatPosition += float(currentTempo * dtm);

        bool isFirstInstrumentAndStaff = true;

        float playLinePosInMeasure = 0.0f;
        int instrumentIndex = 0;
        for (auto instrument : song->instruments)
        {
            player->ChangeInstrument(instrument->midiInstrument.program, instrument->midiInstrument.channel);

            int staffIndex = 0;
            for (auto staff : instrument->staves)
            {
                if (instrumentIndex != 0 || staffIndex != 0)
                    isFirstInstrumentAndStaff = false;

                int measureIndex = 0;
                for (auto measure : staff->measures)
                {
                    float measureBeatPosition = staff->GetMeasureNextBeatPosition(measureIndex,playLineBeatPosition);

                    if (playLineBeatPosition >= measureBeatPosition && playLineBeatPosition <= measure->duration.duration + measureBeatPosition)
                    {
                        if (currentMeasure != measureIndex)
                        {
                            currentMeasure = measureIndex;
                            currentMeasureBeatPosition = measureBeatPosition;
                        }

                        // metronome
                        if (metronomeIsOn && isFirstInstrumentAndStaff)
                        {
                            metronome->OnUpdate(player, measure->timeSignature, measureBeatPosition, playLineBeatPosition, previousPlayLineBeatPosition);
                        }
                    }

                    measure->OnUpdate(player, instrument->midiInstrument.channel, playLineBeatPosition, previousPlayLineBeatPosition, measureBeatPosition, currentTempo);

                    measureIndex++;
                }

                staffIndex++;
            }

            instrumentIndex++;
        }
    }
}