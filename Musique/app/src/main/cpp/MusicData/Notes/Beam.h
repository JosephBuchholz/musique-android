#ifndef MUSIQUE_BEAM_H
#define MUSIQUE_BEAM_H

#include <vector>

#include "Note.h"

class Note;

class Beam
{
public:
    std::vector<Note*> notes; // the notes that this beam is associated with

    // relative to start of measure and top staff line
    float beamStartPositionX = 0.0f;
    float beamStartPositionY = 0.0f;

    float beamEndPositionX = 0.0f;
    float beamEndPositionY = 0.0f;

    enum class BeamType {
        None = 0, Normal, ForwardHook, BackwardHook
    };

    BeamType beamType = BeamType::None;
    int beamLevel = 1;
};

#endif // MUSIQUE_BEAM_H