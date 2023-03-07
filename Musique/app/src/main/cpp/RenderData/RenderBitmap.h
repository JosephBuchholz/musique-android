#ifndef MUSIQUE_RENDERBITMAP_H
#define MUSIQUE_RENDERBITMAP_H

#include "Paint.h"
#include "../AssetID.h"

/**
 * An object that holds data needed to render a bitmap.
 */
struct RenderBitmap {

    RenderBitmap() {}

    RenderBitmap(AssetID assetId, float x, float y, float sx = 1.0f, float sy = 1.0f, Paint paint = Paint())
            : assetId(assetId), x(x), y(y), sx(sx), sy(sy), paint(paint) {
    }

    AssetID assetId = AssetID::None;
    float x = 0.0f;
    float y = 0.0f;
    float sx = 1.0f; // scale x
    float sy = 1.0f; // scale y
    Paint paint = Paint();
};

#endif // MUSIQUE_RENDERBITMAP_H