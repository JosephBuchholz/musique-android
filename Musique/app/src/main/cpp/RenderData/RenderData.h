#pragma once
#include <vector>

#include "Line.h"
#include "Text.h"
#include "RenderBitmap.h"
#include "CubicCurve.h"

struct RenderData {

public:
    void AddLine(const Line& line) {
        Lines.push_back(line);
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

    std::vector<Line> Lines;
    std::vector<Text> Texts;
    std::vector<RenderBitmap> Bitmaps;
    std::vector<CubicCurve> CubicCurves;
};