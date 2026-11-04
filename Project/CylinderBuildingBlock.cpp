#include "CylinderBuildingBlock.h"
#include <algorithm>

CylinderBuildingBlock::CylinderBuildingBlock(const Camera& t_cam) : BaseBuildingBlock(t_cam)
{
}

void CylinderBuildingBlock::InitModel()
{
	model = LoadModelFromMesh(GenMeshCylinder(baseRadius, baseHeight, 10));

	UpdateArrowBounds();
	UpdateTransformMatrix();
}

void CylinderBuildingBlock::UpdateBounds()
{
	Vector3 corners[8] =
	{
		{-baseRadius, 0.0f, -baseRadius},		// 0
		{ baseRadius, 0.0f, -baseRadius},		// 1
		{-baseRadius, baseHeight, -baseRadius}, // 2
		{ baseRadius, baseHeight, -baseRadius}, // 3
		{-baseRadius, 0.0f,  baseRadius},		// 4
		{ baseRadius, 0.0f,  baseRadius},		// 5
		{-baseRadius, baseHeight,  baseRadius}, // 6
		{ baseRadius, baseHeight,  baseRadius}  // 7
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

// https://iquilezles.org/articles/distfunctions/
float CylinderBuildingBlock::SignedDist(Vector3 t_pointPos)
{
	float halfHeight = baseHeight * 0.5f;
	// Invert removes the position, scale and rotation
	Matrix transformInv = MatrixInvert(transform);

	// Convert to local coords
	Vector3 localPoint = Vector3Transform(t_pointPos, transformInv);
	// Local point at center now
	localPoint.y -= halfHeight;

	Vector2 q = { Vector2Length({localPoint.x, localPoint.z}), localPoint.y };
	Vector2 k1 = { baseRadius, halfHeight };
	Vector2 k2 = { baseRadius - baseRadius, 2.0 * halfHeight };
	Vector2 ca = { q.x - std::min(q.x, (q.y < 0.0) ? baseRadius : baseRadius), std::abs(q.y) - halfHeight };
	Vector2 cb = q - k1 + k2 * Clamp(Vector2DotProduct(k1 - q, k2) / Vector2LengthSqr(k2), 0.0, 1.0);
	float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;

	return s * sqrt(std::min(Vector2LengthSqr(ca), Vector2LengthSqr(cb)));
}
