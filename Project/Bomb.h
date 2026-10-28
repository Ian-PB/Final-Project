#pragma once
#include "SphereCollider.h"
#include "Chunk.h"

class World;

class Bomb
{
public:
	Bomb(World& t_world);

	void Draw();
	void Update();

	void Throw(Vector3 t_pos, Vector3 t_vel);
	void Explode();

	Vector3& GetPosRef() { return position; }

private:
	bool active = false;

	Vector3 position;
	Vector3 velocity;
	float radius = 2.0f;
	float explosionRadius = 10.0f;

	void CollisionDetection();
	static const int MAX_COLLISION_ITERATIONS = 6;
	SphereCollider collider;

	void UpdateOccupiedChunks();
	std::vector<Chunk*> occupiedChunks;
	World& WORLD;
};

#include "World.h"
