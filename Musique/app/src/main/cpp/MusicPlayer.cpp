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

                std::pair<int, float> measureData = staff->GetMeasureFromSoundBeatPosition(playLineBeatPosition);
                if (currentMeasure != measureData.first)
                {
                    currentMeasure = measureData.first;
                    currentMeasureBeatPosition = measureData.second;
                    LOGE("currentMeasure: %d, currentMBP: %f, duration: %f, playLineBeat: %f, measureBeatPosition: %f", currentMeasure, currentMeasureBeatPosition, staff->measures[measureData.first]->duration.duration, playLineBeatPosition);
                }

                int measureIndex = 0;
                for (auto measure : staff->measures)
                {
                    //float measureBeatPosition = staff->GetMeasureNextBeatPosition(measureIndex,playLineBeatPosition);
                    //measureBeatPosition = measure->beatPosition;

                    /*if (playLineBeatPosition >= measureBeatPosition && playLineBeatPosition <= measure->duration.duration + measureBeatPosition)
                    {
                        if (currentMeasure != measureIndex)
                        {
                            currentMeasure = measureIndex;
                            currentMeasureBeatPosition = measureBeatPosition;
                            LOGE("currentMeasure: %d, currentMBP: %f, duration: %f, playLineBeat: %f", currentMeasure, currentMeasureBeatPosition, measure->duration.duration, playLineBeatPosition);
                        }


                    }*/

                    if (measureIndex == currentMeasure)
                    {
                        // metronome
                        if (metronomeIsOn && isFirstInstrumentAndStaff)
                        {
                            metronome->OnUpdate(player, measure->timeSignature, currentMeasureBeatPosition, playLineBeatPosition, previousPlayLineBeatPosition);
                        }
                    }

                    measure->OnUpdate(player, measureIndex == currentMeasure, instrument->midiInstrument.channel, playLineBeatPosition, previousPlayLineBeatPosition, currentMeasureBeatPosition, currentTempo, swingTempo, staff->currentVelocity);

                    measureIndex++;
                }

                staffIndex++;
            }

            instrumentIndex++;
        }
    }
}

void MusicPlayer::Reset()
{
    playing = false;
    playLineBeatPosition = 0.0f;
    currentMeasure = 0;
    currentMeasureBeatPosition = 0.0f;
    currentTempo = 120.0f;
    swingTempo = SwingTempo();
}