#include "BeamGroup.h"

float BeamGroup::GetPositionYOnBeam(float positionX) const
{
    if (beamStartPositionX - beamEndPositionX == 0)
    {
        LOGW("Beam has no width");
        return 0.0f;
    }

    float slope = (beamStartPositionY - beamEndPositionY) / (beamStartPositionX - beamEndPositionX); // the slope of the beam

    float yIntercept = beamStartPositionY - (slope * beamStartPositionX); // solve for b (b = y - mx)

    return (slope * positionX) + yIntercept; // y = mx + b
}