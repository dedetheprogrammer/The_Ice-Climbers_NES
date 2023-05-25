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

int mod(int a, int b) {
    return (a%b + b)%b;
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

std::string millis_to_time(int millis) {
    int seconds = millis/1000;      millis %= 1000;
    int minutes = seconds/60;       seconds %= 60;
    int hours = minutes/60;         minutes %= 60;

    std::string time_string = std::to_string(seconds) + ":" + std::to_string(millis);
    if (minutes > 0) time_string = std::to_string(minutes) + ":" + time_string;
    if (hours > 0) time_string = std::to_string(hours) + ":" + time_string;
    return "Tiempo: " + time_string;
}

std::string seconds_to_time(int seconds) {
    int minutes = seconds/60;       seconds %= 60;
    int hours = minutes/60;         minutes %= 60;

    std::string time_string = std::to_string(seconds);
    if (minutes > 0) time_string = std::to_string(minutes) + ":" + time_string;
    if (hours > 0) time_string = std::to_string(hours) + ":" + time_string;
    return "Tiempo: " + time_string;
}