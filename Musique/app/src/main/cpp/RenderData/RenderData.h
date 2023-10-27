#ifndef MUSIQUE_RENDERDATA_H
#define MUSIQUE_RENDERDATA_H

#include <vector>
#include <memory>

#include "Line.h"
#include "Text.h"
#include "RenderBitmap.h"
#include "CubicCurve.h"
#include "SMuFLGlyph.h"
#include "SpannableText.h"
#include "Paint.h"

#include "../MusicDisplayConstants.h"
#include "../MusicData/Types.h"

class Paints
{
public:
    Paints()
    {
        linePaint = Paint();
        linePaint.strokeWidth = 1.4f;

        noteStemPaint = Paint();
        noteStemPaint.strokeWidth = 1.25f;
        noteStemPaint.strokeCap = Paint::Cap::Round;

        noteBeamPaint = Paint();
        noteBeamPaint.strokeWidth = 5.0f;
        noteBeamPaint.strokeCap = Paint::Cap::Butt;
        noteBeamPaint.verticalEnds = true;

        barlinePaint = Paint();
        barlinePaint.strokeWidth = 2.0833f;
        barlinePaint.strokeCap = Paint::Cap::Butt;

        heavyBarlinePaint = Paint();
        heavyBarlinePaint.strokeWidth = 6.6667f;
        heavyBarlinePaint.strokeCap = Paint::Cap::Butt;

        staffLinePaint = Paint();
        staffLinePaint.strokeWidth = DEFAULT_STAFF_LINE_WIDTH;
        staffLinePaint.strokeCap = Paint::Cap::Butt;

        tabSlurPaint = Paint();
        tabSlurPaint.strokeWidth = 1.25f;

        tiePaint = Paint();
        //tiePaint.strokeWidth = 1.25f;
        tiePaint.strokeWidth = 0.6f;
        tiePaint.strokeCap = Paint::Cap::Round;

        textPaint = Paint();
        textPaint.textSize = 30.0f;

        tabTextPaint = Paint();
        tabTextPaint.textSize = 16.0f;
        tabTextPaint.isTablature = true;

        instNameTextPaint = Paint();
        instNameTextPaint.textSize = 9.0f;
        instNameTextPaint.align = Paint::Align::Right;
    }

public:
    Paint linePaint;
    Paint noteStemPaint;
    Paint noteBeamPaint;
    Paint barlinePaint;
    Paint heavyBarlinePaint;

    Paint staffLinePaint;

    Paint tabSlurPaint;
    Paint tiePaint;

    Paint textPaint;
    Paint tabTextPaint;
    Paint instNameTextPaint;
};

/**
 * An object that holds data to render something to the screen.
 */
class RenderData
{

public:
    void AddLine(const Line& line);
    void AddText(const Text& text);
    void AddSpannableText(const SpannableText& text);
    void AddBitmap(const RenderBitmap& bitmap);
    void AddCubicCurve(const CubicCurve& curve);
    void AddGlyph(const SMuFLGlyph& glyph);
    void AddDebugDot(Vec2<float> position);

    std::vector<Line> m_lines;
    std::vector<Text> m_texts;
    std::vector<RenderBitmap> m_bitmaps;
    std::vector<CubicCurve> m_cubicCurves;
    std::vector<SMuFLGlyph> m_SMuFLGlyphs;
    std::vector<SpannableText> m_spannableTexts;

    // the bounds of the area to render in
    float left = 0.0f;
    float right = 1.0f;
    float top = 0.0f;
    float bottom = 1.0f;

    // colors
    Color defaultColor = Color(0xFF000000);
    Color highlightedColor = Color(0xff1188ee);
    Color playedColor = Color(0xff1188ee);

    Paints paints;

    MusicDisplayConstants displayConstants = MusicDisplayConstants();
};

#endif // MUSIQUE_RENDERDATA_H