#ifndef MUSIQUE_RENDERMEASUREMENT_H
#define MUSIQUE_RENDERMEASUREMENT_H

#include "RenderData/SMuFLGlyph.h"
#include "RenderData/Text.h"
#include "RenderData/SpannableText.h"
#include "Collisions/BoundingBox.h"

/**
 * This class handles the measuring of different renderable elements, such as SMuFL glyphs or text.
 */
class RenderMeasurement
{
private:
    RenderMeasurement() {}

public:
    static float MeasureGlyph(const SMuFLGlyph& glyph);
    static float MeasureGlyph(SMuFLID codePoint);
    static BoundingBox GetTextBoundingBox(const Text& text);
    static float MeasureSpannableText(const SpannableText& text);
};

#endif // MUSIQUE_RENDERMEASUREMENT_H