/**
 * This file contains the definition for the `Bend` class.
 */

#ifndef MUSIQUE_BEND_H
#define MUSIQUE_BEND_H

#include "Technique.h"

/**
 * This class represents the bend technique.
 */
class Bend : public Technique
{
public:

    void Render(RenderData& renderData, float notePositionX, float notePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const override;

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

protected:

    virtual void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab) override;

};

#endif //MUSIQUE_BEND_H
