#include "SphereBuildingBlock.h"
#include <algorithm>

SphereBuildingBlock::SphereBuildingBlock(const Camera& t_cam) : BaseBuildingBlock(t_cam)
{
	model = LoadModelFromMesh(GenMeshCube(1, 1, 1));

	UpdateArrowBounds();
	UpdateTransformMatrix();
}

void SphereBuildingBlock::InitModel()
{
	model = LoadModelFromMesh(GenMeshSphere(baseRadius, 10, 10));

	UpdateArrowBounds();
	UpdateTransformMatrix();
}

void SphereBuildingBlock::UpdateBounds()
{
	Vector3 corners[8] =
	{
		{-baseRadius, -baseRadius, -baseRadius}, // 0
		{ baseRadius, -baseRadius, -baseRadius}, // 1
		{-baseRadius,  baseRadius, -baseRadius}, // 2
		{ baseRadius,  baseRadius, -baseRadius}, // 3
		{-baseRadius, -baseRadius,  baseRadius}, // 4
		{ baseRadius, -baseRadius,  baseRadius}, // 5
		{-baseRadius,  baseRadius,  baseRadius}, // 6
		{ baseRadius,  baseRadius,  baseRadius}  // 7
	};

	// Apply the rotation, scale and position
	Vector3 firstPoint = Vector3Transform(corners[0], transform);

	bounds.min = firstPoint;
	bounds.max = firstPoint;

	for (int i = 1; i < 8; i++)
	{
		Vector3 point = Vector3Transform(corners[i], transform);

		// If less than current
		bounds.min = Vector3Min(bounds.min, point);
		// If more than current
		bounds.max = Vector3Max(bounds.max, point);
	}
}

float SphereBuildingBlock::SignedDist(Vector3 t_pointPos)
{
	// Invert removes the position, scale and rotation
	Matrix transformInv = MatrixInvert(transform);

	// Convert to local coords
	Vector3 localPoint = Vector3Transform(t_pointPos, transformInv);

	return Vector3Length(localPoint) - (baseRadius + SIGNED_DIST_SKIN);
}
