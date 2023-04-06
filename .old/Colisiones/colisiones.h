#pragma once

#include "core.h"
#include "mates.h"
#include "game.h"
#include "figuras.h"

enum ColliderType { unknown = -1, player = 0, enemy, projectile, wall, ground }; // No deberiamos especializar los colliders, va a ver comportamientos diferentes? 

class Collider
{
private:
	// ...
public:
	Rectangulo shape;
	ColliderType type;
	Vector2 speed;

	Collider() = default;
	Collider(Rectangulo s, Vector2 v = { 0.0,0.0 }, ColliderType t = unknown) : shape(s), speed(v), type(t) {}
	~Collider() = default;
};

class Collision
{
private:
	// ...
public:
	bool valid;
	ColliderType type;	// Tipo del colisionador contra el que se colisiona
	Vector2 colNormal;	// Direcci�n normal al plano de colisi�n
	// Vector2 suggestedDisplacement;	// Correcci�n para mover al objeto a una posici�n donde est� en espacio libre de colisiones
	// Ref<GameObject> parent;	// Clase padre del colisionador (para llamar a su m�todo). La usa PhysicsManager

	Collision() = default;
	Collision(bool v = false, ColliderType t = unknown, Vector2 norm = { 0.0,0.0 })
		: valid(v), type(t), colNormal(norm) {}
	~Collision() = default;
};

class CollisionHelper
{
private:
	static float adjust_tnc(Rectangulo s1, Vector2 v1, Rectangulo s2, Vector2 v2)
	{
		float tnc = 0.0f;
		Rectangulo s1Copy = Rectangulo(s1);
		Rectangulo s2Copy = Rectangulo(s2);

		for (int i = 0; i < 10; i++)
		{
			tnc += 0.1f;
			s1Copy.Move(s1.x + tnc * v1.x, s1.y + tnc * v1.y);
			s2Copy.Move(s2.x + tnc * v2.x, s2.y + tnc * v2.y);

			if (Collides(s1Copy, s2Copy)) { tnc -= 0.1f; break; }
		}

		return tnc;
	}
public:
	// Collides
	static bool Collides(Rectangulo r1, Rectangulo r2)
	{
		return !(r1.x + r1.width < r2.x		// r1 is to the left of r2
			|| r2.x + r2.width < r1.x		// r1 is to the right of r2
			|| r1.y + r1.height < r2.y		// r1 is above r2 
			|| r2.y + r2.height < r1.y);	// r1 is below r2
	}
	/*
	static bool Collides(Circulo c, Rectangulo r)
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
	static bool Collides(Rectangulo r, Circulo c)
	{
		return Collides(c, r);
	}
	static bool Collides(Circulo c1, Circulo c2)
	{
		return std::pow(c1.radius + c2.radius, 2) >=
			std::pow(c2.x - c1.x, 2) +
			std::pow(c2.y - c1.y, 2);
	}
	*/

	// CollidesV
	static Collision CollidesV(Rectangulo r1, Vector2 v1, Rectangulo r2, Vector2 v2)
	{
		// Caso en el que ya se estaba colisionando al principio del frame
		if (Collides(r1, r2)) return Collision(true);

		// Si no colisiona despu�s de desplazarse, entonces no hace falta calcular nada m�s
		// asumimos los objetos no se mueven a grandes velocidades
		Rectangulo r1Desp = Rectangulo(r1.x + v1.x, r1.y + v1.y, r1.width, r1.height);
		Rectangulo r2Desp = Rectangulo(r2.x + v2.x, r2.y + v2.y, r2.width, r2.height);
		if (!Collides(r1Desp, r2Desp)) return Collision(false);

		float tnc = adjust_tnc(r2, v1, r2, v2);

		r1Desp.Move(r1.x + tnc * v1.x, r1.y + tnc * v1.y);
		r2Desp.Move(r2.x + tnc * v2.x, r2.y + tnc * v2.y);

		// Ahora que tenemos una mayor precisi�n, determinamos el lado de colisi�n
		if (r1Desp.y + r1Desp.height < r2Desp.y) return Collision(true, unknown, { 0.0, -1.0 });
		if (r1Desp.y > r2Desp.y + r2Desp.height) return Collision(true, unknown, { 0.0, 1.0 });
		if (r1Desp.x + r1Desp.width < r2Desp.x) return Collision(true, unknown, { -1.0, 0.0 });
		if (r1Desp.x > r2Desp.x + r2Desp.width) return Collision(true, unknown, { 1.0, 0.0 });

	} 
	/*
	static Collision CollidesV(Circulo c1, Vector2 v1, Circulo c2, Vector2 v2)
	{
		// Caso en el que ya se estaba colisionando al principio del frame
		if (Collides(c1, c2)) return Collision(true);

		// Si no colisiona despu�s de desplazarse, entonces no hace falta calcular nada m�s
		// asumimos los objetos no se mueven a grandes velocidades
		Circulo c1Desp = Circulo(c1.x + v1.x, c1.y + v1.y, c1.radius);
		Circulo c2Desp = Circulo(c2.x + v2.x, c2.y + v2.y, c2.radius);
		if (!Collides(c1Desp, c2Desp)) return Collision(false);

		float tnc = adjust_tnc(c2, v1, c2, v2);

		c1Desp.Move(c1.x + tnc * v1.x, c1.y + tnc * v1.y);
		c2Desp.Move(c2.x + tnc * v2.x, c2.y + tnc * v2.y);

		// Ahora que tenemos una mayor precisi�n, determinamos el lado de colisi�n
		Vector2 normal = Vector2{ c1.x, c1.y } - Vector2{ c2.x, c2.y };
		return Collision(true, unknown, Norm(normal));
	}
	static Collision CollidesV(Rectangulo r, Vector2 vr, Circulo c, Vector2 vc)
	{
		// Caso en el que ya se estaba colisionando al principio del frame
		if (Collides(r, c)) return Collision(true);

		// Si no colisiona despu�s de desplazarse, entonces no hace falta calcular nada m�s
		// asumimos los objetos no se mueven a grandes velocidades
		Rectangulo rDesp = Rectangulo(r.x + vr.x, r.y + vr.y, r.height, r.width);
		Circulo cDesp = Circulo(c.x + vc.x, c.y + vc.y, c.radius);
		if (!Collides(rDesp, cDesp)) return Collision(false);

		float tnc = adjust_tnc(r, vr, c, vc);

		rDesp.Move(r.x + tnc * vr.x, r.y + tnc * vr.y);
		cDesp.Move(c.x + tnc * vc.x, c.y + tnc * vc.y);

		// Ahora que tenemos una mayor precisi�n, determinamos el lado de colisi�n
		Vector2 normal = Vector2{ r.x, r.y } - Vector2{ c.x, c.y };
		return Collision(true, unknown, Norm(normal));
	}
	static Collision CollidesV(Circulo c, Vector2 vc, Rectangulo r, Vector2 vr)
	{
		Collision resCol = CollidesV(r, vr, c, vc);
		// Reverse it
		resCol.colNormal = { -resCol.colNormal.x, -resCol.colNormal.y };
		return resCol;
	}

	// General
	static bool Collides(ColliderShape s1, ColliderShape s2)
	{
		return false;
	}
	static Collision CollidesV(ColliderShape s1, Vector2 v1, ColliderShape s2, Vector2 v2)
	{
		return Collision(false);
	}
	*/
	static Collision CollidesV(Collider c1, Collider c2)
	{
		return CollidesV(c1.shape, c1.speed, c2.shape, c2.speed);
	}

	static Collision CollidesV(Collider c1, Vector2 s1, Collider c2, Vector2 s2)
	{
		return CollidesV(c1.shape, c1.speed, c2.shape, c2.speed);
	}
};