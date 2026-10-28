#pragma once
#include "Collider.h"

struct Triangle;

class SphereCollider : public Collider
{
public:
	void Setup(float t_radius, Vector3 t_position = { 0.0f, 0.0f, 0.0f });
	void Draw() override;

	CollisionInfo CheckAgainstTriangles(const std::vector<Triangle>& t_triangles);

private:
	CollisionInfo SphereTriangle(const Triangle& tri);

	float radius = 0.0f;
};

