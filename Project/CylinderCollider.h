#pragma once
#include "Collider.h"
#include <vector>

class CylinderCollider : public Collider
{
public:
	virtual bool CheckCollision(Mesh& t_mesh, Vector3& t_fixedPos) override;
	virtual void Draw() override;

	void Setup(float t_h, float t_r, int t_slices = 8);

private:
	// Dimensions
	int slices;	// Along both the radius and height
	float height;
	float radius;

	std::vector<Vector3> circleDivisionOffsets; // How much offset the object's center
	std::vector<Vector3> heightDivisionOffsets;	// How much offset the object's center
};

