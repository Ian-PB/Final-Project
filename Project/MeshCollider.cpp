#include "MeshCollider.h"
#include <raymath.h>

std::vector<Vector3*> MeshCollider::trackedPositions;

void MeshCollider::Init(std::vector<Triangle>& t_triangles, Vector3& t_chunkPos)
{
	position = &t_chunkPos;
	allTriangles = &t_triangles;
}

void MeshCollider::AddTrackedPos(Vector3& t_trackedPos)
{
	trackedPositions.push_back(&t_trackedPos);
}

void MeshCollider::Draw()
{
	if (IsKeyReleased(KEY_F1))
		debugMode = !debugMode;

	if (!active || !debugMode)
		return;

	for (const Triangle& t : activeTriangles)
	{
		DrawTriangle3D(t.a, t.b, t.c, { 0, 228, 48, 25 });
	}
}

void MeshCollider::Update()
{
	activeTriangles.clear();
	active = false;

	if (allTriangles == nullptr)
		return;

	for (const Vector3* trackedPos : trackedPositions)
	{
		// Not in bounds
		if (!TargetWithinBounds(trackedPos))
			continue;

		active = true;


		for (const Triangle& t : *allTriangles)
		{
			Triangle worldTri = t;

			worldTri.a = Vector3Add(t.a, *position);
			worldTri.b = Vector3Add(t.b, *position);
			worldTri.c = Vector3Add(t.c, *position);

			float closestDist = std::min(std::min(Vector3Distance(worldTri.a, *trackedPos), Vector3Distance(worldTri.b, *trackedPos)), Vector3Distance(worldTri.c, *trackedPos));

			if (closestDist <= trackingRadius)
			{
				activeTriangles.push_back(worldTri);
			}
		}
	}
}

bool MeshCollider::TargetWithinBounds(const Vector3* trackedPos)
{
	return trackedPos->x >= bounds.min.x - trackingRadius &&
		trackedPos->x <= bounds.max.x + trackingRadius &&
		trackedPos->y >= bounds.min.y - trackingRadius &&
		trackedPos->y <= bounds.max.y + trackingRadius &&
		trackedPos->z >= bounds.min.z - trackingRadius &&
		trackedPos->z <= bounds.max.z + trackingRadius;
}
