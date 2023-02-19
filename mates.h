#pragma once

#include "raylib.h"

#include <cmath>

Vector2 operator-(Vector2 v1, Vector2 v2)
{
	return { v1.x - v2.x, v1.y - v2.y };
}

Vector2 operator+(Vector2 v1, Vector2 v2)
{
	return { v1.x + v2.x, v1.y + v2.y };
}

bool operator==(Vector2 v1, Vector2 v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y);
}

float Mod(Vector2 v)
{
	return std::sqrt(std::pow(v.x, 2.0f) + std::pow(v.y, 2.0f));
}

Vector2 Norm(Vector2 v)
{
	auto mod = Mod(v);
	return { v.x / mod, v.y / mod };
}

float Dot(Vector2 v1, Vector2 v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y);
}