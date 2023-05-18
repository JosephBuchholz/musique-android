/**
 * Contains the Direction class for musical
 * directions (such as rehearsals or metronome marks).
 */

#ifndef MUSIQUE_DIRECTION_H
#define MUSIQUE_DIRECTION_H

#include "../BaseElements/BaseElement.h"

#include "Rehearsal.h"
#include "Words.h"
#include "MetronomeMark.h"
#include "Dynamic.h"
#include "DynamicWedge.h"
#include "BracketDirection.h"
#include "Marker.h"

#include "../Types.h"

#include <vector>

#include <memory>

#include "../../RenderData/RenderData.h"

/**
 * Represents a musical direction (such as a rehearsal or metronome mark).
 */
class Direction : public BaseElement
{
public:
    Direction() {}

    /**
     * Renders this class.
     *
     * @param[out] renderData The RenderData object to render to.
     * @param[in] measurePosition The position of the parent measure.
     * @param[in] offset An offset.
     */
    void Render(RenderData& renderData, Vec2<float> measurePosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

    /**
     * Renders any debug info.
     *
     * @param renderData The RenderData object to render to.
     */
    void RenderDebug(RenderData& renderData) const;

    void UpdateBoundingBox(const Vec2<float>& measurePosition);

public:

    float beatPosition = 0.0f; // the position of the direction in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the direction in the song(not counting repeats) in beats(quarter notes)

    std::vector<Rehearsal> rehearsals;
    std::vector<Words> words;
    std::shared_ptr<MetronomeMark> metronomeMark = nullptr;
    std::vector<Dynamic> dynamics;
    std::shared_ptr<DynamicWedge> dynamicWedge = nullptr;
    std::shared_ptr<BracketDirection> bracketDirection = nullptr;
    std::shared_ptr<Marker> marker = nullptr;

    AboveBelowType placement = AboveBelowType::Above;
};

#endif // MUSIQUE_DIRECTION_H
