package com.randsoft.apps.musique.renderdata

import com.randsoft.apps.musique.AssetID

data class RenderBitmap(
    var assetId: Int = AssetID.None.ordinal,
    var x: Float = 0.0f,
    var y: Float = 0.0f,
    var sx: Float = 1.0f, // scale x
    var sy: Float = 1.0f, // scale y
    var paint: Paint = Paint(0x00000000)
) {
}