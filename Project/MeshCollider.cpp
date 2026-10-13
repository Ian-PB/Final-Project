#include "MeshCollider.h"
#include <raymath.h>

void MeshCollider::Init(Vector3* t_trackedPos, BoundingBox t_meshBounds)
{
	bounds = t_meshBounds;
	trackedPos = t_trackedPos;
}

void MeshCollider::Draw()
{
	for (const Triangle& t : activeTriangles)
	{
		DrawTriangle3D(t.a, t.b, t.c, GREEN);
	}
}

void MeshCollider::Update()
{
	active = TargetInBounds();
	
	if (!active)
		return;

	for (const Triangle& t : allTriangles)
	{
		float closestDist = std::min(std::min(Vector3Distance(t.a, *trackedPos), Vector3Distance(t.b, *trackedPos)), Vector3Distance(t.c, *trackedPos));

		if (closestDist <= trackingRadius)
		{
			activeTriangles.push_back(t);
		}
	}
}

bool MeshCollider::TargetInBounds()
{
	return trackedPos->x >= bounds.min.x && trackedPos->x <= bounds.max.x &&
		trackedPos->y >= bounds.min.y && trackedPos->y <= bounds.max.y &&
		trackedPos->z >= bounds.min.z && trackedPos->z <= bounds.max.z;
}
