#ifndef MUSIQUE_VEC2_H
#define MUSIQUE_VEC2_H

template<typename T>
struct Vec2
{
public:
    Vec2() {}

    Vec2(T x, T y)
        : x(x), y(y) {}

public:

    Vec2<T> operator+(const Vec2<T>& rhs) const
    {
        return { this->x + rhs.x, this->y + rhs.y };
    }

    Vec2<T> operator-(const Vec2<T>& rhs) const
    {
        return { this->x - rhs.x, this->y - rhs.y };
    }

    Vec2<T> operator*(const Vec2<T>& rhs) const
    {
        return { this->x * rhs.x, this->y * rhs.y };
    }

    Vec2<T> operator/(const Vec2<T>& rhs) const
    {
        return { this->x / rhs.x, this->y / rhs.y };
    }
public:
    T x, y;
};

#endif // MUSIQUE_VEC2_H