#pragma once

#include <vector>
/*
class ColliderShape
{
private:
	// ...
public:
	float x;
	float y;
	ColliderShape() = default;
	ColliderShape(Vector2 pos) : x(pos.x), y(pos.y) {}
	ColliderShape(float x, float y) : x(x), y(y) {}
	~ColliderShape() = default;

	void Resize(float newHeight, float newWidth) {}
	void Resize(float newRadius) {}

	void Move(float newX, float newY) {}
	void Draw(Color color = RAYWHITE) {}
};

class Circulo : public ColliderShape
{
private:
	// ...
public:
	float radius;

	Circulo() = default;
	Circulo(Vector2 c, float r) : ColliderShape(c), radius(r) {}
	Circulo(float x, float y, float r) : ColliderShape(x, y), radius(r) {}
	~Circulo() = default;

	void Resize(float newHeight, float newWidth) { radius = newHeight; }
	void Resize(float newRadius) { radius = newRadius; }

	void Move(float newX, float newY) { x = newX; y = newY; }
	void Draw(Color color = RAYWHITE) { DrawCircle(x, y, radius, color); }
};
*/

class Rectangulo /*: public ColliderShape*/
{
private:
	// ...
public:
	float x;
	float y;
	float width;
	float height;

	Rectangulo() = default;
	Rectangulo(Vector2 pos, Vector2 size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}
	Rectangulo(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}
	Rectangulo(Rectangle r) : x(r.x), y(r.y), width(r.width), height(r.height) {}
	~Rectangulo() = default;

	void Resize(float newHeight, float newWidth) { height = newHeight; width = newWidth; }
	void Resize(float newRadius) { height = newRadius; width = newRadius; }

	void Move(float newX, float newY) { x = newX; y = newY; }
	void Move(Vector2 position) { x = position.x; y = position.y; }
	void Draw(Color color = RED) { DrawRectangleLinesEx(Rectangle{x, y, width, height}, 3, color); }
	void Print() { std::cout << "Rectangle{ x: " << x << " y: " << y << " width: " << width << " height: " << height << "}" << std::endl; }
};