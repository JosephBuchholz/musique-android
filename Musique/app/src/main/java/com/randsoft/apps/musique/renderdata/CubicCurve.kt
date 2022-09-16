package com.randsoft.apps.musique.renderdata

// a cubic bezier curve
data class CubicCurve(
    var x1: Float = 0.0f,
    var y1: Float = 0.0f,
    var x2: Float = 0.0f,
    var y2: Float = 0.0f,
    var x3: Float = 0.0f,
    var y3: Float = 0.0f,
    var x4: Float = 0.0f,
    var y4: Float = 0.0f,
    var paint: Paint = Paint(0x00000000.toInt())
) {
}