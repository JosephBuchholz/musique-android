/**
 * This files defines one of the base element classes, `LineElement`.
 */

#ifndef MUSIQUE_LINEELEMENT_H
#define MUSIQUE_LINEELEMENT_H

#include "BaseElement.h"
#include "Types.h"
#include "../RenderData/Paint.h"

/**
 * This class represents a element that is (or has) a line (or multiple lines).
 */
class LineElement : public virtual BaseElement
{
public:

    /**
     * Updates the given paint to match the line attributes of this class.
     *
     * @param[out] paint The paint object to be modified.
     */
    void ModifyPaint(Paint& paint) const;

public:
    LineType lineType = LineType::Solid;

    // only for dashed lines
    float dashLength = 0.0f;

    // only for dashed or dotted lines
    float dashSpaceLength = 0.0f;
};

#endif // MUSIQUE_LINEELEMENT_H