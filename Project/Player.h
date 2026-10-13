#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Lighting.h"
#include "CylinderCollider.h"

class World;

class Player
{
public:
	Player(World& t_world);
	void Init();

	void Update();
	void Draw();
	void Draw2D();

	Camera& GetCameraRef() { return camera; }
private:
	Vector3 GetCameraPos() { return Vector3Add(collider.GetPos(), cameraPosOffset); }
	void SetupCamera();
	void CameraLook();
	void Movement();
	Camera camera;
	Vector3 cameraPosOffset = { 0.0f, 0.0f, 0.0f }; // Setup in Init()

	Vector3 position;
	Vector3 forward;
	float moveSpeed = 10.0f;

	float cameraSensitivity = 0.003f;
	float cameraYaw = 0.0f;
	float cameraPitch = 0.0f;

	// Ray from camera
	void ShootRay();
	Ray ray;
	Vector3 rayStart;
	RayCollision rayCollision;
	bool breaking = true;
	float rayRadius = 2.0f;
	float radiusChange = 0.5f;

	World& WORLD;

	CylinderCollider collider;
	float height = 3.0f;
	float radius = 1.0f;

	// Debug
	bool displayDebugElements = false;

	Light& light;

	void SetRandLight(Vector3 pos);
	Light& randLight;
};

#include "World.h"