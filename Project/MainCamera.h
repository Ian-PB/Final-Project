#pragma once
#include <raylib.h>
#include <raymath.h>

class MainCamera
{
public:
	MainCamera(Vector3& t_followPosition, Vector3 t_positionOffset);

	void UpdatePos();
	void Update();

	Vector3 GetPos() { return Vector3Add(followPosition, positionOffset); }
	Camera& GetCameraRef() { return camera; } 
	Vector3 GetForward() const { return forward; }
	float GetYaw() const { return cameraYaw; }

private:
	Camera camera;
	Vector3& followPosition;
	Vector3 positionOffset;

	Vector3 forward;

	float cameraSensitivity = 0.003f;
	float cameraYaw = 0.0f;
	float cameraPitch = 0.0f;
};

