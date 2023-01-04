#pragma once

template<typename T>
struct Vec2
{
    Vec2(T x, T y)
        : x(x), y(y) {}

    T x, y;
};