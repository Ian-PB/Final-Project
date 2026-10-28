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

	void SetBreaking(bool b) { breaking = b; }

	Vector3& GetPosRef() { return position; }

private:
	bool ExplosionTimer();
	float timer = 0.0f;
	const float TIMER_DURATION = 3.0f;

	bool active = false;
	bool timerStarted = false;

	Vector3 position;
	Vector3 velocity;
	float radius = 0.5f;
	float explosionRadius = 10.0f;

	bool breaking = true;

	void CollisionDetection();
	static const int MAX_COLLISION_ITERATIONS = 6;
	SphereCollider collider;

	void UpdateOccupiedChunks();
	std::vector<Chunk*> occupiedChunks;
	World& WORLD;
};

#include "World.h"
