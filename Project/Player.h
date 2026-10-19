#pragma once
#include <raylib.h>
#include "Lighting.h"
#include "CapsuleCollider.h"
#include "MainCamera.h"
#include <memory>

class World;

class Player
{
public:
	Player(World& t_world);
	void Init();

	void Update();
	void Draw();
	void Draw2D();

	Vector3& GetPosRef() { return position; }
	Camera& GetCameraRef() { return camera->GetCameraRef(); }
private:
	void Movement();

	Vector3 position;
	float moveSpeed = 10.0f;

	std::shared_ptr<MainCamera> camera;

	// Ray from camera
	void ShootRay();
	Ray ray;
	Vector3 rayStart;
	RayCollision rayCollision;
	bool breaking = true;
	float rayRadius = 2.0f;
	float radiusChange = 0.5f;

	World& WORLD;

	void CollisionDetection();
	CapsuleCollider collider;
	static const int MAX_COLLISION_ITERATIONS = 6;
	float height = 3.0f;
	float radius = 1.0f;

	// Debug
	bool displayDebugElements = false;

	Light& light;

	void SetRandLight(Vector3 pos);
	Light& randLight;
};

#include "World.h"