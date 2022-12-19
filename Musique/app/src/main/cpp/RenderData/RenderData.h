#pragma once
#include <vector>
#include <memory>

#include "Line.h"
#include "Text.h"
#include "RenderBitmap.h"
#include "CubicCurve.h"
#include "SMuFLGlyph.h"

struct RenderData {

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

    void AddBitmap(const RenderBitmap& bitmap) {
        Bitmaps.push_back(bitmap);
    }

    void AddCubicCurve(const CubicCurve& curve) {
        CubicCurves.push_back(curve);
    }

    void AddGlyph(const SMuFLGlyph& glyph) {
        SMuFLGlyphs.push_back(glyph);
    }

    std::vector<std::shared_ptr<Line>> Lines;
    std::vector<Text> Texts;
    std::vector<RenderBitmap> Bitmaps;
    std::vector<CubicCurve> CubicCurves;
    std::vector<SMuFLGlyph> SMuFLGlyphs;
};