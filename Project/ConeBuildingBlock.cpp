#include "ConeBuildingBlock.h"
#include <algorithm>

ConeBuildingBlock::ConeBuildingBlock(const Camera& t_cam) : BaseBuildingBlock(t_cam)
{
}

void ConeBuildingBlock::InitModel()
{
	model = LoadModelFromMesh(GenMeshCone(baseRadius, baseHeight, 10));

	UpdateArrowBounds();
	UpdateTransformMatrix();
}

std::shared_ptr<BaseBuildingBlock> ConeBuildingBlock::CreateClone()
{
	return std::make_shared<ConeBuildingBlock>(*this);
}

void ConeBuildingBlock::UpdateBounds()
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
float ConeBuildingBlock::SignedDist(Vector3 t_pointPos)
{
	float halfHeight = baseHeight * 0.5f;
	// Invert removes the position, scale and rotation
	Matrix transformInv = MatrixInvert(transform);

	// Convert to local coords
	Vector3 localPoint = Vector3Transform(t_pointPos, transformInv);
	// Local point at center now
	localPoint.y -= halfHeight;

	float slantLength = sqrt(baseRadius * baseRadius + baseHeight * baseHeight);
	Vector2 c = { baseRadius / slantLength, baseHeight / slantLength };

	Vector2 q = { baseHeight * c.x / c.y, -baseHeight };
	Vector2 w = { Vector2Length({ localPoint.x, localPoint.z }), localPoint.y - halfHeight };

	Vector2 a = w - Vector2Scale(q, Clamp(Vector2DotProduct(w, q) / Vector2LengthSqr(q), 0.0f, 1.0f));
	Vector2 b = w - Vector2Multiply(q, { Clamp(w.x / q.x, 0.0f, 1.0f), 1.0f });

	float k = (q.y > 0.0f) ? 1.0f : -1.0f;
	float d = std::min( Vector2LengthSqr(a), Vector2LengthSqr(b));
	float s = std::max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y) );

	return sqrt(d) * ((s > 0.0f) ? 1.0f : (s < 0.0f ? -1.0f : 0.0f));
}
