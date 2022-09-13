package com.randsoft.apps.musique.renderdata

data class Text(
    var text: String = "",
    var x: Float = 0.0f,
    var y: Float = 0.0f,
    var paint: Paint = Paint(0x00000000.toInt())
) {
}