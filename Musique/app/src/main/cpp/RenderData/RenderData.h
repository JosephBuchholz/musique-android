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

class Paints
{
public:
    Paints()
    {
        linePaint = Paint();
        linePaint.strokeWidth = 1.4f;

        noteStemPaint = Paint();
        noteStemPaint.strokeWidth = 0.8333f;
        noteStemPaint.strokeCap = Paint::Cap::Round;

        noteBeamPaint = Paint();
        noteBeamPaint.strokeWidth = 5.0f;
        noteBeamPaint.strokeCap = Paint::Cap::Butt;
        noteBeamPaint.verticalEnds = true;

        barlinePaint = Paint();
        barlinePaint.strokeWidth = 1.25f;
        barlinePaint.strokeCap = Paint::Cap::Butt;

        heavyBarlinePaint = Paint();
        heavyBarlinePaint.strokeWidth = 5.0f;
        heavyBarlinePaint.strokeCap = Paint::Cap::Butt;

        tabSlurPaint = Paint();
        tabSlurPaint.strokeWidth = 1.25f;

        tiePaint = Paint();
        tiePaint.strokeWidth = 1.25f;

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
    ~RenderData()
    {
        /*for (int i = 0; i < Lines.size(); i++)
        {
            delete Lines[i];
        }*/
        Lines.clear();
    }

public:

    std::shared_ptr<Line> AddLine(float startX, float startY, float endX, float endY, Paint paint = Paint()) {
        //Line line = Line(startX, startY, endX, endY, paint);
        std::shared_ptr<Line> line = std::make_shared<Line>(startX, startY, endX, endY, paint);
        Lines.push_back(line);
        return line;
    }

    std::shared_ptr<Line> AddLine(std::shared_ptr<Line> line) {
        Lines.push_back(line);
        return line;
    }

    void AddText(const Text& text) {
        Texts.push_back(text);
    }

    void AddSpannableText(std::shared_ptr<SpannableText> text) {
        SpannableTexts.push_back(text);
    }

    void AddBitmap(const RenderBitmap& bitmap) {
        Bitmaps.push_back(bitmap);
    }

    void AddCubicCurve(const CubicCurve& curve) {
        CubicCurves.push_back(curve);
    }

    void AddGlyph(const SMuFLGlyph& glyph) {
        SMuFLGlyphs.push_back(glyph);
    }

    void AddDebugDot(float x, float y)
    {
        std::shared_ptr<Line> line = std::make_shared<Line>();

        line->startX = x;
        line->startY = y;

        line->endX = x + 10.0f;
        line->endY = y + 10.0f;

        line->paint = Paint(0xFFFF00FF);
        line->paint.strokeWidth = 10.0f;

        Lines.push_back(line);
    }

    std::vector<std::shared_ptr<Line>> Lines;
    std::vector<Text> Texts;
    std::vector<RenderBitmap> Bitmaps;
    std::vector<CubicCurve> CubicCurves;
    std::vector<SMuFLGlyph> SMuFLGlyphs;
    std::vector<std::shared_ptr<SpannableText>> SpannableTexts;

    Color defaultColor = Color(0xFF000000);
    Color highlightedColor = Color(0xff1188ee);
    Color playedColor = Color(0xff1188ee);

    Paints paints;

    MusicDisplayConstants displayConstants = MusicDisplayConstants();
};

#endif // MUSIQUE_RENDERDATA_H