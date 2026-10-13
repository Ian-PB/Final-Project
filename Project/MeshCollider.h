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
	void Init(Vector3* t_trackedPos, BoundingBox t_bounds);
	void Draw();
	void Update();

	bool TargetInBounds();

	void SetActive(bool a) { active = a; }

private:
	bool active = false;

	BoundingBox bounds;

	Vector3* trackedPos;
	float trackingRadius = 15.0f;;

	std::vector<Triangle> allTriangles;
	std::vector<Triangle> activeTriangles;
};

