#include "RenderData.h"

void RenderData::AddLine(const Line& line)
{
    m_lines.push_back(line);
}

void RenderData::AddText(const Text& text)
{
    m_texts.push_back(text);
}

void RenderData::AddSpannableText(const SpannableText& text)
{
    m_spannableTexts.push_back(text);
}

void RenderData::AddBitmap(const RenderBitmap& bitmap)
{
    m_bitmaps.push_back(bitmap);
}

void RenderData::AddCubicCurve(const CubicCurve& curve)
{
    m_cubicCurves.push_back(curve);
}

void RenderData::AddGlyph(const SMuFLGlyph& glyph)
{
    m_SMuFLGlyphs.push_back(glyph);
}

void RenderData::AddDebugDot(Vec2<float> position)
{
    Line line = Line();

    line.start = position;
    line.end = position + Vec2<float>{ 10.f, 10.0f };

    line.paint = Paint(0xFFFF00FF);
    line.paint.strokeWidth = 10.0f;

    m_lines.push_back(line);
}