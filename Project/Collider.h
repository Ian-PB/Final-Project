#pragma once
#include <raylib.h>

class Collider
{
public:
	virtual bool CheckCollision(Mesh& t_mesh, Vector3& t_fixedPos) = 0;
	virtual void Draw() = 0;

protected:
	Vector3 position;

	// Debug
	bool debugMode = false;
	const float DEBUG_POINT_RADIUS = 0.05f;
};