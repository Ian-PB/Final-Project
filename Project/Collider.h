#pragma once
#include <raylib.h>

struct Triangle
{
	Vector3 a;
	Vector3 b;
	Vector3 c;

	Vector3 normal;
};

class Collider
{
public:
	// virtual bool CheckCollision(Mesh t_mesh, Matrix t_matrix, Vector3& t_fixedPos) = 0;

	virtual void Draw() = 0;

	Vector3 GetPos() const { return position; }
	void SetPos(Vector3 t_newPos) { position = t_newPos; }

protected:
	void ToggleDebug() 
	{
		if (IsKeyReleased(KEY_F1))
			debugMode = !debugMode;
	}

	Vector3 position = { 0.0f, 0.0f, 0.0f };

	// Debug
	bool debugMode = false;
	const float DEBUG_POINT_RADIUS = 0.05f;
};