#pragma once
#include "Paint.h"
#include "../AssetID.h"

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