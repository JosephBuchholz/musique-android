#ifndef MUSIQUE_METRONOME_H
#define MUSIQUE_METRONOME_H

#include "../MusicData/Measures/TimeSignature.h"
#include "Player.h"

class Metronome
{
public:

    void OnUpdate(std::shared_ptr<Player> player, const TimeSignature& timeSignature, float measureBeatPosition, float playLineBeatPosition, float previousPlayLineBeatPosition);
};

#endif //MUSIQUE_METRONOME_H
