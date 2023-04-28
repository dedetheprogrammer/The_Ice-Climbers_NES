#pragma once
#ifndef RAYLIB_EXT_H
#define RAYLIB_EXT_H

#include <cmath>
#include <iostream>
#include "raylib.h"

// ----------------------------------------------------------------------------
// Math extension
// ----------------------------------------------------------------------------
template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

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

#endif