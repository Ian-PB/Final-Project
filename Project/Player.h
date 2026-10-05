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

	Camera& GetCameraRef() { return camera; }
private:
	void SetupCamera();
	void CameraLook();
	void Movement();
	Camera camera;

	Vector3 position;
	Vector3 forward;
	float moveSpeed = 5.0f;

	float cameraSensitivity = 0.003f;
	float cameraYaw = 0.0f;
	float cameraPitch = 0.0f;

	// Ray from camera
	void ShootRay();
	Ray ray;
	Vector3 rayStart;
	RayCollision rayCollision;
	bool breaking = true;
	float rayRadius = 20.0f;

	World& WORLD;
};

#include "World.h"