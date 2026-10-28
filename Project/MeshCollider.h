#pragma once
#include <raylib.h>
#include <vector>
#include "Collider.h"

class MeshCollider
{
public:
	void Init(std::vector<Triangle>& t_triangles, Vector3& t_chunkPos);
	static void AddTrackedPos(Vector3& t_trackedPos);

	void Draw();
	void Update();

	std::vector<Triangle>& GetTriangles() { return activeTriangles; }

	void SetBounds(BoundingBox t_bounds) { bounds = t_bounds; }
	bool TargetWithinBounds(const Vector3* trackedPos);

	void SetActive(bool a) { active = a; }

private:
	bool active = false;
	bool debugMode = false;

	Vector3* position = nullptr;
	BoundingBox bounds;

	// Share between all mesh colliders
	static std::vector<Vector3*> trackedPositions;

	float trackingRadius = 7.0f;;

	std::vector<Triangle>* allTriangles = nullptr;
	std::vector<Triangle> activeTriangles;
};

