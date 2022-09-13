package com.randsoft.apps.musique.renderdata

data class Paint(
    var color: Int
) {
    var strokeWidth: Float = 1.0f
    var strokeCap: Int = 0
    var textSize: Float = 10.0f
}