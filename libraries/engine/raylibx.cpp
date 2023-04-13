#include "raylibx.h"

Vector2 operator+(Vector2 a, Vector2 b) {
    return {a.x + b.x, a.y + b.y};
}

void operator+=(Vector2& a, const Vector2& b) {
    a.x += b.x;
    a.y += b.y;
}

Vector2 operator-(Vector2 a) {
    return {-a.x, -a.y};
}

Vector2 operator-(Vector2 a, Vector2 b) {
    return {a.x - b.x, a.y - b.y};
}

void operator-=(Vector2 a, Vector2 b) {
    a.x -= b.x;
    a.y -= b.y;
}

Vector2 operator*(Vector2 a, Vector2 b) {
    return {a.x * b.x, a.y * b.y};
}

Vector2 operator/(Vector2 a, Vector2 b) {
    return {a.x/b.x, a.y/b.y};
}

bool operator==(Vector2 a, Vector2 b) {
    return a.x == b.x && a.y == b.x;
}

bool operator!=(Vector2 a, Vector2 b) {
    return !(a == b);
}

std::ostream& operator<<(std::ostream& os, Vector2 v) {
    return os << v.x << "," << v.y;
}

float dot(Vector2 a, Vector2 b) {
   return a.x*b.x + a.y*b.y; 
}

float mod(Vector2 v) {
    return std::sqrt(v.x*v.x + v.y*v.y);
}

Vector2 abs(Vector2 v) {
    return {std::abs(v.x), std::abs(v.y)};
}

Vector2 nor(Vector2 v, float n) {
    return v*n/mod(v);
}

Vector2 reflect(Vector2 wi, Vector2 n) {
  return wi - 2.f * dot(wi, n) * n;
}