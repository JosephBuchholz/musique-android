package com.randsoft.apps.musique.renderdata

data class TextSpan(
    var startIndex: Int = 0,
    var endIndex: Int = 0,
    var paint: Paint = Paint(0x00000000)
)

class SpannableText(
    var text: String = "",
    var x: Float = 0.0f,
    var y: Float = 0.0f,
    var mainPaint: Paint = Paint(0x00000000),
    var spans: Array<TextSpan> = emptyArray()
)