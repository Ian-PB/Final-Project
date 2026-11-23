#pragma once
#include <raylib.h>
#include <vector>
#include "NetworkObject.h"

#include "SphereCollider.h"
#include "Chunk.h"

class World;

class Bullet : public NetworkObject
{
public:
	Bullet(int t_owner, World& t_world);

	void Draw();
	void Update();

	void Shoot(Vector3 t_pos, Vector3 t_dir);

	void SetBreaking(bool b) { breaking = b; }
	Vector3& GetPosRef() { return position; }

	bool IsActive() const { return active; }

	// Networking
	virtual std::vector<char> Serialise() override;
	virtual NetworkObject* UpdateFromDeserialisation(const char* t_data) override;

private:
	bool active = false;

	Vector3 position = {0.0f, 0.0f, 0.0f};
	Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	float speed = 125.0f;
	float radius = 0.5f;

	int damage = 10;

	// Collision
	void CollisionDetection();
	static const int MAX_COLLISION_ITERATIONS = 6;
	SphereCollider collider;

	// On hit variables
	void OnHit();
	bool hasCollided = false;
	Vector3 hitPosition = { 0.0f, 0.0f, 0.0f };
	bool breaking = true;
	float explosionRadius = 2.0f;

	void UpdateOccupiedChunks();
	std::vector<Chunk*> occupiedChunks;
	World& WORLD;
};

#include "World.h"