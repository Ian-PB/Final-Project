#pragma once
#include <raylib.h>
#include <vector>
#include "Collider.h"

class MeshCollider
{
public:
	void Init(Vector3& t_trackedPos, std::vector<Triangle>& t_triangles, Vector3& t_chunkPos);
	void Draw();
	void Update();

	std::vector<Triangle>& GetTriangles() { return activeTriangles; }

	void SetBounds(BoundingBox t_bounds) { bounds = t_bounds; }
	bool TargetWithinBounds();

	void SetActive(bool a) { active = a; }

private:
	bool active = false;
	bool debugMode = false;

	Vector3* position = nullptr;
	BoundingBox bounds;

	Vector3* trackedPos;
	float trackingRadius = 15.0f;;

	std::vector<Triangle>* allTriangles = nullptr;
	std::vector<Triangle> activeTriangles;
};

