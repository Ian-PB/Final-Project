#pragma once
#include <raylib.h>
#include "Collider.h"
#include <vector>

struct Triangle;

struct CollisionInfo
{
	bool hit = false;
	Vector3 normal = { 0.0f, 0.0f, 0.0f };
	float penetration = 0.0f;
};

class CapsuleCollider : public Collider
{
public:
	void Setup(float t_height, float t_radius, Vector3 t_position = {0.0f, 0.0f, 0.0f});
	void Draw() override;

	CollisionInfo CheckAgainstTriangles(const std::vector<Triangle>& t_triangles);

	void SetPos(Vector3 t_p) { position = t_p; }
	Vector3 GetPos() const { return position; }

private:
	void TestPoints(Vector3 t_capsulePoint, Vector3 t_trianglePoint, float& t_minDistSq, Vector3& t_closestCapsule, Vector3& t_closestTriangle);
	CollisionInfo CapsuleTriangle(Vector3 t_start, Vector3 t_end, const Triangle& tri);
	Vector3 ClosestPointToTriangle(Vector3 t_point, Vector3 t_triA, Vector3 t_triB, Vector3 t_triC);
	float ShortestSegmentToSegmentDistSq(Vector3 t_p1, Vector3 t_q1, Vector3 t_p2, Vector3 t_q2, Vector3& t_closest1, Vector3& t_closest2);

	// Position at base
	float height;
	float radius;
};

#include "MeshCollider.h"
