#pragma once

struct Paint {

    Paint() {}

    Paint(int color)
            : color(color) {
    }

    Paint(int color, const Paint& paint)
            : color(color) {
        strokeWidth = paint.strokeWidth;
        strokeCap = paint.strokeCap;
        textSize = paint.textSize;
    }

    enum class Cap {
        Butt = 0, Round, Square
    };

    int color = 0xff000000;

    float strokeWidth = 1.0f;
    Cap strokeCap = Cap::Butt;
    float textSize = 10.0f;
};