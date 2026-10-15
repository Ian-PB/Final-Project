#pragma once
#include <raylib.h>
#include "Collider.h"
#include <vector>

struct Triangle;

class CapsuleCollider : public Collider
{
public:
	void Setup(float t_height, float t_radius, Vector3 t_position = {0.0f, 0.0f, 0.0f});
	void Draw() override;

	bool CheckAgainstTriangles(const std::vector<Triangle>& t_triangles);

	void SetPos(Vector3 t_p) { position = t_p; }
	Vector3 GetPos() const { return position; }

private:
	bool CapsuleTriangle(Vector3 t_start, Vector3 t_end, const Triangle& tri);
	Vector3 ClosestPointToTriangle(Vector3 t_point, Vector3 t_triA, Vector3 t_triB, Vector3 t_triC);
	float ShortestSegmentToSegmentDistSq(Vector3 p1, Vector3 q1, Vector3 p2, Vector3 q2);

	// Position at base
	float height;
	float radius;
};

#include "MeshCollider.h"
