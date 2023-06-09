#pragma once
#ifndef RAYLIB_EXT_H
#define RAYLIB_EXT_H

#include <cmath>
#include <iostream>
#include <random>
#include "raylib.h"

#define COYOTEBROWN CLITERAL(Color){ 186, 143, 80, 255 }
#define JELLYFISH CLITERAL(Color){ 91, 191, 222, 255 }
#define JELLYBEANBLUE CLITERAL(Color){ 66, 117, 133, 255 }

// ----------------------------------------------------------------------------
// Math extension
// ----------------------------------------------------------------------------
template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }
template <typename T> int sign(T val) {
    if (val < 0) {
        return -1;
    } else if (val > 0) {
        return 1;
    } else {
        return 0;
    }
}

// ----------------------------------------------------------------------------
// Vector extension
// ----------------------------------------------------------------------------

bool operator==(Vector2 a, Vector2 b);
bool operator!=(Vector2 a, Vector2 b);
Vector2 operator+(Vector2 a, Vector2 b);
template <typename T> Vector2 operator-(Vector2 a, T b) { return {a.x - b, a.y - b}; }
Vector2 operator-(Vector2 a);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 a, Vector2 b);
Vector2 operator/(Vector2 a, Vector2 b);
template <typename T> Vector2 operator*(Vector2 a, T b) { return {(float)(a.x * b), (float)(a.y * b)}; }
template <typename T> Vector2 operator*(T a, Vector2 b) { return b*a; }
template <typename T> Vector2 operator/(Vector2 a, T b) { return {a.x/b, a.y/b}; }
template <typename T> Vector2 operator/(T a, Vector2 b) { return {a/b.x, a/b.y}; }

void operator+=(Vector2& a, const Vector2& b);
void operator-=(Vector2 a, Vector2 b);
template <typename T> void operator*=(Vector2& a, T b) { a.x *= b; a.y *= b; }

float dot(Vector2 a, Vector2 b);
float mod(Vector2 v);
int mod(int a, int b);
Vector2 abs(Vector2 v);
Vector2 nor(Vector2 v, float n = 1);
Vector2 reflect(Vector2 wi, Vector2 n);

std::ostream& operator<<(std::ostream& os, Vector2 v);

// ----------------------------------------------------------------------------
// Time extension
// ----------------------------------------------------------------------------
std::string millis_to_time(int millis);
std::string seconds_to_time(int seconds);

#endif