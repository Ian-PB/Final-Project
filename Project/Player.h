#pragma once
#include <raylib.h>

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
	void SetupCamera();
	void CameraLook();
	void Movement();
	Camera camera;

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
};

#include "World.h"