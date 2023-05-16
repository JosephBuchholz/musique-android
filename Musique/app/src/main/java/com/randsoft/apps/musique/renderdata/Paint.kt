package com.randsoft.apps.musique.renderdata

data class Paint(
    var color: Int
) {
    var strokeWidth: Float = 1.0f
    var strokeCap: Int = 0
    var textSize: Float = 10.0f
    var glyphSizeFactor: Float = 1.0f // this scales the size of the glyph
    var isItalic: Boolean = false
    var isBold: Boolean = false
    var align: Int = 0
    var isTablature: Boolean = false
    var isAntiAlias: Boolean = false
    var strikeThruText: Boolean = false

    var rotateDegrees: Float = 0.0f // how much to rotate by in degrees

    var centerVertically: Boolean = false
    var centerHorizontally: Boolean = false

    var hasBackground: Boolean = false
    var backgroundColor: Int = (0xFFFFFFFF).toInt()
    var backgroundPadding: Float = 0.0f

    var useMusicFont: Boolean = false
    var verticalEnds: Boolean = false

    var isDottedLine: Boolean = false
    var isDashedLine: Boolean = false

    // for dashed lines
    var dashLength: Float = 10.0f

    // for dotted and dashed lines
    var dashSpaceLength: Float = 10.0f

    // for dotted lines
    var dotRadius: Float = 1.0f
}