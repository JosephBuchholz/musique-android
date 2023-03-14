/**
 * Contains the Direction class for musical
 * directions (such as rehearsals or metronome marks).
 */

#ifndef MUSIQUE_DIRECTION_H
#define MUSIQUE_DIRECTION_H

#include "../BaseElement.h"

#include "Rehearsal.h"
#include "Words.h"
#include "MetronomeMark.h"
#include "Dynamic.h"
#include "DynamicWedge.h"

#include "../Types.h"

#include <vector>

#include <memory>

#include "../../RenderData/RenderData.h"

/**
 * Represents a musical direction (such as a rehearsal or metronome mark).
 */
class Direction : public BaseElement {
public:
    Direction() {}

    /**
     * Renders this class
     *
     * @param[out] renderData The RenderData object to render to
     * @param[in] measurePositionX The x position of the parent measure
     * @param[in] measurePositionY The y position of the parent measure
     * @param[in] offsetX offset in x direction
     * @param[in] offsetY offset in y direction
     */
    void Render(RenderData& renderData, float measurePositionX, float measurePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const;

public:

    float beatPosition = 0.0f; // the position of the direction in the measure in beats(quarter notes)
    float beatPositionInSong = 0.0f; // the position of the direction in the song(not counting repeats) in beats(quarter notes)

    std::vector<Rehearsal> rehearsals;
    std::vector<Words> words;
    std::shared_ptr<MetronomeMark> metronomeMark = nullptr;
    std::vector<Dynamic> dynamics;
    std::shared_ptr<DynamicWedge> dynamicWedge = nullptr;

    AboveBelowType placement = AboveBelowType::Above;
};

#endif // MUSIQUE_DIRECTION_H
