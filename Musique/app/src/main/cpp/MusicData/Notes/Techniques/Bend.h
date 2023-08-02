/**
 * This file contains the definition for the `Bend` class.
 */

#ifndef MUSIQUE_BEND_H
#define MUSIQUE_BEND_H

#include "Technique.h"

#include "../../BaseElements/TextElement.h"

// TODO: allow a bend with both a release and pre-bend
/**
 * This class represents the bend technique.
 */
class Bend : public Technique
{
public:

    void Render(RenderData& renderData, Vec2<float> notePosition) const override;

    BoundingBox GetBoundingBox() const override { return BoundingBox(); }

    Vec2<float> GetDimensions() const override;

    SMuFLID GetSMuFLID() const;

    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY) override;

    void OnPlay(std::shared_ptr<Player> player, int channel) override;
    void OnStop(std::shared_ptr<Player> player, int channel) override;
    void OnUpdate(std::shared_ptr<Player> player, float beatPositionRelativeToNote, float noteDuration, int channel) override;

public:

    enum class BendType
    {
        None = 0, Normal, PreBend, Release
    } bendType = BendType::Normal;

    enum class DisplayType
    {
        None = 0, Curved, Angled
    } displayType = DisplayType::Curved;

    float alterSemitones = 0.0f;

    float firstBeat = 0.25f; // percentage of the duration taken by the starting of the bend
    float lastBeat = 0.75f; // percentage of the duration taken by the ending of the bend

    TextElement text; // the text displayed for the bend (ex: "1/2" or "full") (only for curved bends)

    bool withBar = false; // whether the bend is done with a whammy bar

    Vec2<float> arrowPosition = { 0.0f, 0.0f };
    Vec2<float> arrowReleasePosition = { 0.0f, 0.0f };
    Vec2<float> tailPosition = { 0.0f, 0.0f };
};

#endif //MUSIQUE_BEND_H
