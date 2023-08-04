package com.randsoft.apps.musique.renderdata

data class Line(
    var startX: Float = 0.0f,
    var startY: Float = 0.0f,
    var endX: Float = 0.0f,
    var endY: Float = 0.0f,
    var paint: Paint = Paint(0x00000000)
) {
}