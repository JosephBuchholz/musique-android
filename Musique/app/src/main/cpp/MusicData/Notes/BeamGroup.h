/**
 * This file contains the definition of the `BeamGroup` class.
 */

#ifndef MUSIQUE_BEAMGROUP_H
#define MUSIQUE_BEAMGROUP_H

#include <vector>

#include "Beam.h"

class BeamGroup
{
public:

    float GetPositionYOnBeam(float positionX) const;

public:

    std::vector<Note*> notes; // the notes that this beam group is associated with

    std::vector<Beam> beams;

    // relative to start of measure and top staff line
    float beamStartPositionX = 0.0f;
    float beamStartPositionY = 0.0f;

    float beamEndPositionX = 0.0f;
    float beamEndPositionY = 0.0f;

    bool isAboveNote = false;
};

#endif // MUSIQUE_BEAMGROUP_H