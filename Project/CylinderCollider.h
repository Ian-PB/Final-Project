#pragma once
#include "Collider.h"
#include <vector>

class CylinderCollider : public Collider
{
public:
	virtual bool CheckCollision(Mesh t_mesh, Matrix t_matrix, Vector3& t_fixedPos) override;
	virtual void Draw() override;

	void Setup(float t_h, float t_r, int t_slices = 8);
	float GetHeight() const { return height; }

private:
	void DrawDebug();
	void DrawPoints();
	void DrawRays();

	// Dimensions
	int slices;	// Along both the radius and height
	float height;
	float radius;

	// Points of collision / starting points of rays
	std::vector<Vector3> bottomCircleDivisionOffsets; // How much offset the object's center
	std::vector<Vector3> topCircleDivisionOffsets; // How much offset the object's center
	std::vector<Vector3> heightDivisionOffsets;	// How much offset the object's center

	// Ray directions
	std::vector<Vector3> directionsAway;

	// Rays
	void GetRays();
	std::vector<Ray> rays;

	static const Vector3 RAY_UP_DIR;
	static const Vector3 RAY_DOWN_DIR;
	static const float MAX_COLLISION_DISTANCE;
};

