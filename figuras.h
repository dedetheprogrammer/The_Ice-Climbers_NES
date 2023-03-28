#pragma once

#include "mates.h"

#include <vector>

struct Circle
{
	float x;
	float y;
	float radius;

	Circle() = default;
	Circle(Vector2 c, float r) : x(c.x), y(c.y), radius(r) {}
	Circle(float x, float y, float r) : x(x), y(y), radius(r) {}
	~Circle() = default;
};
struct Polygon
{
	float x;
	float y;
	std::vector<Vector2> corners;
	std::vector<Vector2> normals;
	Rectangle AABB;

	Polygon() = default;
	Polygon(Rectangle rec)
	{
		x = rec.x + rec.width / 2;
		y = rec.y + rec.height / 2;
		// add the vertices
		corners.push_back({ rec.x, rec.y });							// p0 ---- p3
		corners.push_back({ rec.x, rec.y + rec.height });				// |        |
		corners.push_back({ rec.x + rec.width, rec.y + rec.height });	// |        |
		corners.push_back({ rec.x + rec.width, rec.y });				// p1 ---- p2
		// add the normals (x and y axis)
		normals.push_back({ 1.0f,0.0f });
		normals.push_back({ 0.0f,1.0f });
	}

	Polygon(Vector2 c, std::vector<Vector2> v)
	{
		// Store the center and corners
		x = c.x;
		y = c.y;
		corners = v;

		// Determine the normal vectors for the sides of the shape
		Vector2 edge = corners[corners.size()] - corners[0];
		Vector2 perp = { edge.y, -edge.x };
		perp = Norm(perp);
		normals.push_back(perp);

		// Search the outer bounds of the polygon's AABB rectangle
		Vector2 topLeft = corners[0], botRight = corners[0];

		// Repeat for the remaining edges
		for (uint32_t i = 1; i < corners.size(); i++)
		{
			edge = corners[i] - corners[i - 1];
			perp = { edge.y, -edge.x };
			perp = Norm(perp);

			// Avoid quasi-duplicates
			uint32_t j = 0;
			for (; j < normals.size(); j++)
				if ((normals[j].x - perp.x < 0.001) && (normals[j].y - perp.y < 0.001))
					break;

			if (j == normals.size()) normals.push_back(perp);

			topLeft.y = std::max(topLeft.y, corners[i].y);
			topLeft.x = std::min(topLeft.x, corners[i].x);

			botRight.y = std::min(botRight.y, corners[i].y);
			botRight.x = std::max(botRight.x, corners[i].x);
		}
		// Define the AABB rectangle based on topLeft and botRight bounds
		AABB = { topLeft.x, topLeft.y, (botRight - topLeft).x, (botRight - topLeft).y };
	}
	~Polygon() = default;

	void rotate(float radians)
	{
		// Normalize the rotation to the complete circle
		radians = std::fmod(std::abs(radians), 2.0f * PI);
		// Pre-calculate cosine and sin of the given angle in radians
		float cos = std::cos(radians);
		float sin = std::sin(radians);

		// Search the outer bounds of the polygon's AABB rectangle
		Vector2 topLeft = corners[0], botRight = corners[0];
		// Rotate the corners
		for (auto& corner : corners)
		{
			// Shifting the pivot point to the origin
			// and the given points accordingly
			float x_shift = corner.x - x;
			float y_shift = corner.y - y;

			// Calculating the rotated point co-ordinates
			// and shifting it back
			corner.x = (x_shift * cos - y_shift * sin) + x;
			corner.y = (x_shift * sin - y_shift * cos) + y;

			// Keep track of the bounds of the AABB rectangle
			topLeft.y = std::max(topLeft.y, corner.y);
			topLeft.x = std::min(topLeft.x, corner.x);

			botRight.y = std::min(botRight.y, corner.y);
			botRight.x = std::max(botRight.x, corner.x);
		}
		// Define the AABB rectangle based on the new topLeft and botRight bounds
		AABB = { topLeft.x, topLeft.y, (botRight - topLeft).x, (botRight - topLeft).y };
		
		// Rotate the normals
		for (auto& normal : normals)
		{
			// Normals can be rotated around origin, so no shift is needed
			normal.x = normal.x * cos - normal.y * sin;
			normal.y = normal.x * sin - normal.y * cos;
		}
	}
	void DrawOutline(Color color)
	{
		DrawRectangleRec(AABB, DARKGRAY);
		Vector2 xy = { x, y };

		uint32_t size = corners.size();
		DrawLineV(xy + corners[size - 1], xy + corners[0], color);
		for (uint32_t i = 1; i < size; i++)
		{
			DrawLineV(xy + corners[i - 1], xy + corners[i], color);
		}
	}
};