#pragma once
#include <raylib.h>

class Player
{
public:
	void Init();

	void Update();

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
};

