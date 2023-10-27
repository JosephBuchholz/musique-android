#include "MusicPlayer.h"

#include "MidiPlayer.h"

MusicPlayer::MusicPlayer()
{
    player = std::make_shared<MidiPlayer>();
    metronome = std::make_shared<Metronome>();
}

void MusicPlayer::OnStart()
{
    player->ChangeInstrument((int)Player::InstrumentSound::AcousticGrandPiano, 0);
}

void MusicPlayer::OnPlay()
{
    LOGI("MusicPlayer: OnPlay");
    playing = true;
}

void MusicPlayer::OnStop()
{
    LOGI("MusicPlayer: OnStop");
    playing = false;
    player->StopAllNotes();
}

void MusicPlayer::OnUpdate(double dt, const std::shared_ptr<Song>& song)
{
    double dts = dt / 1000.0; // delta time in seconds
    double dtm = dts / 60.0; // delta time in minutes

    if (playing)
    {
        float previousTempo = currentTempo;

        float previousPlayLineBeatPosition = playLineBeatPosition;
        playLineBeatPosition += float(currentTempo * tempoPercentage * dtm);

        bool isFirstInstrumentAndStaff = true;

        float playLinePosInMeasure = 0.0f;
        int instrumentIndex = 0;
        for (const auto& instrument : song->instruments)
        {
            player->ChangeInstrument(instrument->midiInstrument.program, instrument->midiInstrument.channel);

            int staffIndex = 0;
            for (const auto& staff : instrument->staves)
            {
                if (instrumentIndex != 0 || staffIndex != 0)
                    isFirstInstrumentAndStaff = false;

                std::pair<int, float> measureData = staff->GetMeasureFromSoundBeatPosition(playLineBeatPosition, song->endingGroups);
                if (currentMeasure != measureData.first)
                {
                    currentMeasure = measureData.first;
                    currentMeasureBeatPosition = measureData.second;
                    //LOGD("currentMeasure: %d, currentMBP: %f, duration: %f, playLineBeat: %f, measureBeatPosition: %f", currentMeasure, currentMeasureBeatPosition, staff->measures[measureData.first]->duration.duration, playLineBeatPosition);
                }

                int measureIndex = 0;
                for (const auto& measure : staff->measures)
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

                    measure->OnUpdate(player, measureIndex == currentMeasure, instrument->midiInstrument.channel, playLineBeatPosition, previousPlayLineBeatPosition, currentMeasureBeatPosition, currentTempo, swingTempo, staff->currentVelocity, instrument->capo);

                    measureIndex++;
                }

                staffIndex++;
            }

            instrumentIndex++;
        }


        if (previousTempo != currentTempo)
            OnTempoChangedCallback(currentTempo);
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

void MusicPlayer::TravelToVisualBeatPosition(float beatPosition, const std::shared_ptr<Song>& song)
{
    std::shared_ptr<Instrument> instrument = song->instruments[0];
    if (instrument)
    {
        std::shared_ptr<Staff> staff = instrument->staves[0];
        if (staff)
        {
            playLineBeatPosition = staff->GetSoundBeatPositionFromVisualBeatPosition(beatPosition, song->endingGroups);
        }
    }

    for (auto instrument : song->instruments)
    {
        for (const auto& staff : instrument->staves)
        {
            for (const auto& measure : staff->measures)
            {
                for (const auto& soundEvent : measure->soundEvents)
                {
                    if (soundEvent->beatPosition + measure->beatPosition <= beatPosition)
                    {
                        float previousTempo = currentTempo;
                        soundEvent->ModifyTempo(currentTempo);
                        if (previousTempo != currentTempo)
                            OnTempoChangedCallback(currentTempo);
                        soundEvent->ModifySwingTempo(swingTempo);
                        soundEvent->ModifyVelocity(staff->currentVelocity);
                    }
                }
            }
        }
    }
}

void MusicPlayer::TravelToSoundBeatPosition(float beatPosition)
{
    playLineBeatPosition = beatPosition;
}

void MusicPlayer::SetVolume(float volume)
{
    if (player)
        player->SetVolume(volume);
}

void MusicPlayer::SetTempoPercentage(float tp)
{
    tempoPercentage = tp;
}