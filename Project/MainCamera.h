#pragma once
#include <raylib.h>
#include <raymath.h>

class MainCamera
{
public:
	MainCamera(Vector3& t_followPosition, float t_heightOffset);

	void UpdatePos();
	void Update();

	Vector3 GetPos() { return Vector3Add(followPosition, positionOffset); }
	Camera& GetCameraRef() { return camera; } 
	Vector3 GetForward() const { return forward; }
	float GetYaw() const { return cameraYaw; }

	void SetHeight(float t_height);

private:
	Camera camera;
	Vector3& followPosition;
	Vector3 positionOffset = { 0.0f, 0.0f, 0.0f };
	float heightOffset = 0.0f;

	Vector3 forward;

	float cameraSensitivity = 0.003f;
	float cameraYaw = 0.0f;
	float cameraPitch = 0.0f;
	float height = 0.0f;
};

