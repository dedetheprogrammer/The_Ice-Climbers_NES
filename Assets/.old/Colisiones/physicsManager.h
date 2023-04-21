#pragma once

#include "colisiones.h"
#include "game.h"
#include <list>

class Force
{
private:
	// ...
public:
	Vector2 acceleration;
	bool hasArea;
	Rectangulo shape;

	Force(Vector2 a = {0.0,0.0}, bool ha = false, Rectangulo s = Rectangulo())
		: acceleration(a), hasArea(ha), shape(s) {}
	~Force() = default;
};
/*
class PhysicsManager
{
private:
	// ...
public:
	std::list<Ref<Player>> players = {};
	std::list<Ref<Enemy>> enemies = {};
	std::list<Ref<Projectile>> projectiles = {};
	std::list<Ref<GameObject>> otherObjects = {};

	std::list<Force> forces = {};

	PhysicsManager()
	{
		forces.push_back(Force({ 0.0,0.98 }));
	}
	~PhysicsManager() = default;

	void AddPlayer(Player player) { players.push_back(CreateRef<Player>(player)); }
	void AddEnemy(Enemy enemy) { enemies.push_back(CreateRef<Enemy>(enemy)); }
	void AddProjectile(Projectile projectile) { projectiles.push_back(CreateRef<Projectile>(projectile)); }
	void AddOtherObject(GameObject other) { otherObjects.push_back(CreateRef<GameObject>(other)); }

	void RemovePlayer(Player player) { players.remove(CreateRef<Player>(player)); }
	void RemoveEnemy(Enemy enemy) { enemies.remove(CreateRef<Enemy>(enemy)); }
	void RemoveProjectile(Projectile projectile) { projectiles.remove(CreateRef<Projectile>(projectile)); }
	void RemoveOtherObject(GameObject other) { otherObjects.remove(CreateRef<GameObject>(other)); }

	void CalculateCollisions()
	{
		auto collisions = std::list<Collision>();
		// Player vs Enemy
		for (auto& player : players)
			for (auto& enemy : enemies)
			{
				Collision c = CollisionHelper::CollidesV(player->collider, enemy->collider);
				if (c.valid)
				{
					//c.parent = player;
					c.type = enemy->collider.type;
					collisions.push_back(c);
					player->ApplyCollision(c);
				}
			}
		// Player vs Projectile
		for (auto& player : players)
			for (auto& projectile : projectiles)
			{
				Collision c = CollisionHelper::CollidesV(player->collider, projectile->collider);
				if (c.valid)
				{
					//c.parent = player;
					c.type = projectile->collider.type;
					collisions.push_back(c);
					player->ApplyCollision(c);
				}
			}
		// Player vs World
		for (auto& player : players)
			for (auto& other : otherObjects)
			{
				Collision c = CollisionHelper::CollidesV(player->collider, other->collider);
				if (c.valid)
				{
					//c.parent = player;
					c.type = other->collider.type;
					collisions.push_back(c);
					player->ApplyCollision(c);
				}
			}

		// Enemy vs Projectile
		for (auto& enemy : enemies)
			for (auto& other : otherObjects)
			{
				Collision c = CollisionHelper::CollidesV(enemy->collider, other->collider);
				if (c.valid)
				{
					//c.parent = enemy;
					c.type = other->collider.type;
					collisions.push_back(c);
					enemy->ApplyCollision(c);
				}
			}
		// Enemy vs World
		for (auto& enemy : enemies)
			for (auto& other : otherObjects)
			{
				Collision c = CollisionHelper::CollidesV(enemy->collider, other->collider);
				if (c.valid)
				{
					//c.parent = enemy;
					c.type = other->collider.type;
					collisions.push_back(c);
					enemy->ApplyCollision(c);
				}
			}

		// Projectile vs World
		for (auto& projectile : projectiles)
			for (auto& other : otherObjects)
			{
				Collision c = CollisionHelper::CollidesV(projectile->collider, other->collider);
				if (c.valid)
				{
					//c.parent = projectile;
					c.type = other->collider.type;
					collisions.push_back(c);
					projectile->ApplyCollision(c);
				}
			}
		// Aplicar colisiones a los objetos de la lista
		
		//for (auto& collision : collisions)
		//	collision.parent->ApplyCollision(collision);
		
	}

	void CalculateForces()
	{
		for (auto& force : forces)
		{
			for (auto& player : players)
				if (!force.hasArea || CollisionHelper::Collides(force.shape, player->collider.shape))
					player->ApplyAcceleration(force.acceleration);

			for (auto& enemy : enemies)
				if (!force.hasArea || CollisionHelper::Collides(force.shape, enemy->collider.shape))
					enemy->ApplyAcceleration(force.acceleration);
		}
	}
};*/