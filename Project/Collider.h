#pragma once
#include <raylib.h>

// Sources I saw after doing the cylinder collider: 
// https://blog.bearcats.nl/capsule-triangle-sweep/
// https://cseweb.ucsd.edu/classes/sp19/cse291-d/Files/CSE291_13_CollisionDetection.pdf

class Collider
{
public:
	virtual bool CheckCollision(Mesh t_mesh, Matrix t_matrix, Vector3& t_fixedPos) = 0;
	virtual void Draw() = 0;

	Vector3 GetPos() const { return position; }
	void SetPos(Vector3 t_newPos) { position = t_newPos; }

protected:
	Vector3 position = { 0.0f, 0.0f, 0.0f };

	// Debug
	bool debugMode = false;
	const float DEBUG_POINT_RADIUS = 0.05f;
};