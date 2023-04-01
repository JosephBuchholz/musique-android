/**
 * This file contains the definition of the `Dynamic` class.
 */

#ifndef MUSIQUE_DYNAMIC_H
#define MUSIQUE_DYNAMIC_H

#include <string>

#include "../TextualElement.h"

#include "../../Collisions/Vec2.h"
#include "../Types.h"

#include "../../RenderData/RenderData.h"

/**
 * This class represents a dynamic direction (such as mf or p).
 */
class Dynamic : public TextualElement {
    friend class Song;
    friend class MusicXMLParser;

public:

    /**
     * Updates the position and size of this object's bounding box.
     *
     * @param parentPosition The position of the parent.
     */
    void UpdateBoundingBox(const Vec2<float> &parentPosition);

    /**
     * Renders this class.
     *
     * @param[out] renderData The RenderData object to render to.
     * @param[in] measurePositionX The x position of the parent measure.
     * @param[in] measurePositionY The y position of the parent measure.
     * @param[in] offsetX offset in x direction.
     * @param[in] offsetY offset in y direction.
     */
    void Render(RenderData& renderData, float measurePositionX, float measurePositionY, float offsetX = 0.0f, float offsetY = 0.0f) const;

    SMuFLID GetDynamicSMuFLID() const;

protected:
    void CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, Vec2<float> defaultPos);

public:

    enum class DynamicType
    {
        None = 0, Piano, Pianissimo, OtherPiano, Forte, Fortissimo, OtherForte, MezzoPiano, MezzoForte, SF, SFP, SFPP, FP, RF, RFZ, SFZ, SFFZ, FZ, N, PF, SFZP, OtherDynamic
    } type = DynamicType::MezzoForte;

    static SMuFLID GetDynamicSMuFLID(DynamicType type);

    // indicates how many p's are in a piano marking and how many f's are in a forte marking (only needed if the dynamic type is OtherPiano or OtherForte).
    int dynamicIntensity = 1;

    std::string displayString = ""; // the characters that this dynamic should display (should mainly be used if the dynamic type is OtherDynamic)

    AboveBelowType placement = AboveBelowType::None;

    // -- Positioning Attributes --

    Vec2<float> position = { 0.0f, 0.0f }; // relative to measure

protected:

    // -- MusicXML Positioning Attributes --

    Vec2<float> defaultPosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
    Vec2<float> relativePosition = { INVALID_FLOAT_VALUE, INVALID_FLOAT_VALUE };
};

#endif // MUSIQUE_DYNAMIC_H
