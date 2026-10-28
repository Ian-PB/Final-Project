#pragma once
#include <raylib.h>
#include "Collider.h"

struct Triangle;

class CapsuleCollider : public Collider
{
public:
	void Setup(float t_height, float t_radius, Vector3 t_position = {0.0f, 0.0f, 0.0f});
	void Draw() override;

	CollisionInfo CheckAgainstTriangles(const std::vector<Triangle>& t_triangles);

private:
	void TestPoints(Vector3 t_capsulePoint, Vector3 t_trianglePoint, float& t_minDistSq, Vector3& t_closestCapsule, Vector3& t_closestTriangle);
	CollisionInfo CapsuleTriangle(Vector3 t_start, Vector3 t_end, const Triangle& tri);
	float ShortestSegmentToSegmentDistSq(Vector3 t_p1, Vector3 t_q1, Vector3 t_p2, Vector3 t_q2, Vector3& t_closest1, Vector3& t_closest2);

	// Position at base
	float height;
	float radius;
};

#include "MeshCollider.h"
