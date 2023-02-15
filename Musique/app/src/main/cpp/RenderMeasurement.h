#pragma once

#include "RenderData/SMuFLGlyph.h"
#include "RenderData/Text.h"
#include "RenderData/SpannableText.h"

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
    static float MeasureText(const Text& text);
    static float MeasureSpannableText(const SpannableText& text);
};