package com.randsoft.apps.musique.renderdata

class RenderData {

    var lines: Array<Line> = emptyArray()
    var texts: Array<Text> = emptyArray()
    var bitmaps: Array<RenderBitmap> = emptyArray()
    var cubicCurves: Array<CubicCurve> = emptyArray()
    var glyphs: Array<SMuFLGlyph> = emptyArray()
    var spannableTexts: Array<SpannableText> = emptyArray()
    var scaling: Scaling = Scaling()

    // the bounds of the area to render in
    var left: Float = 0.0f
    var right: Float = 1.0f
    var top: Float = 0.0f
    var bottom: Float = 1.0f
}