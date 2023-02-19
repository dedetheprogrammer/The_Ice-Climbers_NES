#pragma once

#include "figuras.h"

class CollisionHelper
{
private:
	static Vector2 FindMaxMinProjection(Polygon poly, Vector2 axis)
	{
		float projection = Dot(Vector2({ poly.x, poly.y }) + poly.corners[0], axis);
		float min = projection;
		float max = projection;
		for (uint32_t i = 1; i < poly.corners.size(); i++)
		{
			projection = Dot(Vector2({ poly.x, poly.y }) + poly.corners[i], axis);
			min = min < projection ? min : projection;
			max = max > projection ? max : projection;
		}
		return { min, max };
	}
public:
	// Rectangles
	static bool Collides(Rectangle r1, Rectangle r2)
	{
		return !(r1.x + r1.width < r2.x		// r1 is to the left of r2
			|| r2.x + r2.width < r1.x		// r1 is to the right of r2
			|| r1.y + r1.height < r2.y		// r1 is above r2 
			|| r2.y + r2.height < r1.y);	// r1 is below r2
	}
	static bool Collides(Circle c, Rectangle r)
	{
		Vector2 clamp;
		clamp.x = (c.x < r.x) ?
			r.x : (c.x > r.x + r.width) ?
			r.x + r.width : c.x;

		clamp.y = (c.y < r.y) ?
			r.y : (c.y > r.y + r.height) ?
			r.y + r.height : c.y;

		return std::pow(c.radius, 2) >=
			std::pow(c.x - clamp.x, 2) +
			std::pow(c.y - clamp.y, 2);
	}
	static bool Collides(Rectangle r, Circle c)
	{
		return Collides(c, r);
	}

	// Circles
	static bool Collides(Circle c1, Circle c2)
	{
		return std::pow(c1.radius + c2.radius, 2) >=
			std::pow(c2.x - c1.x, 2) +
			std::pow(c2.y - c1.y, 2);
	}
	static bool Collides(Polygon p, Circle c)
	{
		// If the outer bounding boxes don't collide,
		// there is no collision
		if (!Collides(p.AABB, c)) return false;

		// Check the first polygon's normals
		for (auto normal : p.normals)
		{
			// Determine the minimum and maximum projection 
			// for both polygons
			Vector2 mm1 = FindMaxMinProjection(p, normal);
			// For a circle min is always center - radius
			// and max is always center + radius
			float cProj = Dot({ c.x,c.y }, normal);
			Vector2 mm2 = { cProj - c.radius, cProj + c.radius };
			// Test for separation (as soon as we find a separating axis, we
			// know there is no possibility of collision, so we can exit early)
			if (mm1.y < mm2.x || mm2.y < mm1.x) return false;
		}
		// If we reach this point, no separating axis was found
		// and the two polygons are colliding
		return true;
	}
	static bool Collides(Circle c, Polygon p)
	{
		return Collides(p, c);
	}

	// Polygons
	static bool Collides(Polygon p1, Polygon p2)
	{
		// If the outer bounding boxes don't collide,
		// there is no collision
		if (!Collides(p1.AABB, p2.AABB)) return false;

		// Check the first polygon's normals
		for (auto normal : p1.normals)
		{
			// Determine the minimum and maximum projection 
			// for both polygons
			auto mm1 = FindMaxMinProjection(p1, normal);
			auto mm2 = FindMaxMinProjection(p2, normal);
			// Test for separation (as soon as we find a separating axis,
			// we know there is no possibility of collision, so we can 
			// exit early)
			if (mm1.y < mm2.x || mm2.y < mm1.x) return false;
		}
		// Repeat for the second polygon's normals
		for (auto normal : p2.normals)
		{
			// Determine the minimum and maximum projection 
			// for both polygons
			auto mm1 = FindMaxMinProjection(p1, normal);
			auto mm2 = FindMaxMinProjection(p2, normal);
			// Test for separation (as soon as we find a separating axis, we
			// know there is no possibility of collision, so we can exit early)
			if (mm1.y < mm2.x || mm2.y < mm1.x) return false;
		}
		// If we reach this point, no separating axis was found
		// and the two polygons are colliding
		return true;
	}
};