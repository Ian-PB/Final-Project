#pragma once
#include <raylib.h>
#include <vector>

struct Triangle
{
	Vector3 a;
	Vector3 b;
	Vector3 c;

	Vector3 normal;
};

class MeshCollider
{
public:
	void Init(Vector3& t_trackedPos, std::vector<Triangle>& t_triangles, Vector3& t_chunkPos);
	void Draw();
	void Update();

	void SetBounds(BoundingBox t_bounds) { bounds = t_bounds; }
	bool TargetWithinBounds();

	void SetActive(bool a) { active = a; }

private:
	bool active = false;

	Vector3* position = nullptr;
	BoundingBox bounds;

	Vector3* trackedPos;
	float trackingRadius = 15.0f;;

	std::vector<Triangle>* allTriangles = nullptr;
	std::vector<Triangle> activeTriangles;
};

