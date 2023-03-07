#ifndef MUSIQUE_VEC2_H
#define MUSIQUE_VEC2_H

template<typename T>
struct Vec2
{
    Vec2(T x, T y)
        : x(x), y(y) {}

    T x, y;
};

#endif // MUSIQUE_VEC2_H