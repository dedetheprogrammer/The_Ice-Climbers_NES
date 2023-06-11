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

// ----------------------------------------------------------------------------
// Time extension
// ----------------------------------------------------------------------------
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
    int minutes = seconds/60;
    seconds %= 60;
    int hours = minutes/60;
    minutes %= 60;

    std::string time_string;
    std::ostringstream oss;
    oss.fill('0');

    // Agregar horas, minutos y segundos
    oss << std::setw(2) << hours << ":";
    oss << std::setw(2) << minutes << ":";
    oss << std::setw(2) << seconds;

    time_string = oss.str();
    return time_string;
}

// ----------------------------------------------------------------------------
// String treatment
// ----------------------------------------------------------------------------
std::string fill_string(std::string s, int width) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(width) << s;
    return oss.str();
}