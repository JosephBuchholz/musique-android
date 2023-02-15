package com.randsoft.apps.musique.renderdata

data class Paint(
    var color: Int
) {
    var strokeWidth: Float = 1.0f
    var strokeCap: Int = 0
    var textSize: Float = 10.0f
    var isItalic: Boolean = false
    var isBold: Boolean = false
    var align: Int = 0
    var isTablature: Boolean = false
    var isAntiAlias: Boolean = false
    var strikeThruText: Boolean = false
    var centerTextVertically: Boolean = false
    var useMusicFont: Boolean = false
}