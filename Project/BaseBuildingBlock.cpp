#include "BaseBuildingBlock.h"
#include <raymath.h>
#include <algorithm>

const float BaseBuildingBlock::MIN_SCALE = 0.02f;

BaseBuildingBlock::BaseBuildingBlock(const Camera& t_cam) : CAMERA(t_cam)
{
	model = LoadModelFromMesh(GenMeshCube(1, 1, 1));

	UpdateArrowBounds();
	UpdateTransformMatrix();
}

void BaseBuildingBlock::Draw(EditTypes t_editType)
{
	DrawModel(model, { 0, 0, 0 }, 1.0f, WHITE);
	DrawModelWires(model, { 0, 0, 0 }, 1.0f, DARKGRAY);

	if (!selected)
		return;

	switch (t_editType)
	{
	case EditTypes::Move:
		MoveDraw();
		break;
	case EditTypes::Rotate:
		RotateDraw();
		break;
	case EditTypes::Scale:
		ScaleDraw();
		break;

	default:
		break;
	}
}

void BaseBuildingBlock::Update(EditTypes t_editType)
{
	// Only update is selected
	if (!selected)
		return;

	// Set selected axis to the hovered axis
	if (IsMouseButtonPressed(0))
	{
		selectedAxis = hoveredAxis;

		if (selectedAxis != Axis::None)
		{
			isDragging = true;
			EnableCursor();
		}
	}
	else if (IsMouseButtonReleased(0))
	{
		selectedAxis = Axis::None;

		isDragging = false;
		DisableCursor();
	}

	switch (t_editType)
	{
	case EditTypes::Move:
		MoveUpdate();
		break;
	case EditTypes::Rotate:
		RotateUpdate();
		break;
	case EditTypes::Scale:
		ScaleUpdate();
		break;

	default:
		break;
	}
}

RayCollision BaseBuildingBlock::CheckIfClicked(const Ray& ray)
{
	RayCollision collision = GetRayCollisionMesh(ray, model.meshes[0], transform);

	return collision;
}

bool BaseBuildingBlock::IsPointInside(Vector3 t_point)
{
	// Invert removes the position, scale and rotation
	Matrix transformInv = MatrixInvert(transform);
	// Convert to local coords
	Vector3 localPoint = Vector3Transform(t_point, transformInv);

	bool inAxisX = localPoint.x >= -0.5f && localPoint.x <= 0.5f;
	bool inAxisY = localPoint.y >= -0.5f && localPoint.y <= 0.5f;
	bool inAxisZ = localPoint.z >= -0.5f && localPoint.z <= 0.5f;

	return inAxisX && inAxisY && inAxisZ;
}

void BaseBuildingBlock::Move()
{
	Vector2 mouseMove = GetMouseDelta() * changeSpeedMult;

	if (selectedAxis == Axis::X)
	{
		position.x -= mouseMove.x;
	}
	else if (selectedAxis == Axis::Y)
	{
		position.y -= mouseMove.y;
	}
	else if (selectedAxis == Axis::Z)
	{
		position.z -= mouseMove.x;
	}

	UpdateArrowBounds();
	UpdateTransformMatrix();
}

void BaseBuildingBlock::MoveUpdate()
{
	hoveredAxis = CheckArrowSelectedOn();

	if (selectedAxis == Axis::None)
		return;

	Move();
}

void BaseBuildingBlock::MoveDraw()
{
	// X
	Vector3 endPosX = position;
	endPosX.x += arrowLength + (scale.x * 0.5f);
	Vector3 pointStartPosX = endPosX;
	pointStartPosX.x += arrowHeadHeight;

	if (hoveredAxis == Axis::X)
	{
		DrawCylinderEx(position, endPosX, lineWidth, lineWidth, 10, MAROON);
		DrawCylinderEx(pointStartPosX, endPosX, 0, arrowHeadBase, 10, MAROON);
	}
	else
	{
		DrawCylinderEx(position, endPosX, lineWidth, lineWidth, 10, RED);
		DrawCylinderEx(pointStartPosX, endPosX, 0, arrowHeadBase, 10, RED);
	}

	// Y
	Vector3 endPosY = position;
	endPosY.y += arrowLength + (scale.y * 0.5f);
	Vector3 pointStartPosY = endPosY;
	pointStartPosY.y += arrowHeadHeight;

	if (hoveredAxis == Axis::Y)
	{
		DrawCylinderEx(position, endPosY, lineWidth, lineWidth, 10, DARKBLUE);
		DrawCylinderEx(pointStartPosY, endPosY, 0, arrowHeadBase, 10, DARKBLUE);
	}
	else
	{
		DrawCylinderEx(position, endPosY, lineWidth, lineWidth, 10, BLUE);
		DrawCylinderEx(pointStartPosY, endPosY, 0, arrowHeadBase, 10, BLUE);
	}

	// Z
	Vector3 endPosZ = position;
	endPosZ.z += arrowLength + (scale.z * 0.5f);
	Vector3 pointStartPosZ = endPosZ;
	pointStartPosZ.z += arrowHeadHeight;

	if (hoveredAxis == Axis::Z)
	{
		DrawCylinderEx(position, endPosZ, lineWidth, lineWidth, 10, DARKGREEN);
		DrawCylinderEx(pointStartPosZ, endPosZ, 0, arrowHeadBase, 10, DARKGREEN);
	}
	else
	{
		DrawCylinderEx(position, endPosZ, lineWidth, lineWidth, 10, GREEN);
		DrawCylinderEx(pointStartPosZ, endPosZ, 0, arrowHeadBase, 10, GREEN);
	}
}

void BaseBuildingBlock::Rotate()
{
	Vector2 mouseMove = GetMouseDelta() * changeSpeedMult;

	if (selectedAxis == Axis::X)
	{
		rotation.x -= mouseMove.y;
	}
	else if (selectedAxis == Axis::Y)
	{
		rotation.y -= mouseMove.x;
	}
	else if (selectedAxis == Axis::Z)
	{
		rotation.z -= mouseMove.y;
	}

	UpdateArrowBounds();
	UpdateTransformMatrix();
}

void BaseBuildingBlock::RotateUpdate()
{
	hoveredAxis = CheckCircleSelectedOn();

	if (selectedAxis == Axis::None)
		return;

	Rotate();
}

void BaseBuildingBlock::RotateDraw()
{
	// Draw 3 circle outlines (Up, Left, Right) encasing object
	float largestScale = std::max(scale.x, std::max(scale.y, scale.z));
	float radius = largestScale * ROTATION_CIRCLE_RADIUS_MULT;

	// X Rotation
	if (hoveredAxis == Axis::X)
	{
		DrawCircle3D(position, radius, { 0, 1, 0 }, 90, BLACK);
	}
	else
	{
		DrawCircle3D(position, radius, { 0, 1, 0 }, 90, RED);
	}

	// Y Rotation
	if (hoveredAxis == Axis::Y)
	{
		DrawCircle3D(position, radius, { 1, 0, 0 }, 90, BLACK);
	}
	else
	{
		DrawCircle3D(position, radius, { 1, 0, 0 }, 90, BLUE);
	}

	// Z Rotation
	if (hoveredAxis == Axis::Z)
	{
		DrawCircle3D(position, radius, { 0, 0, 0 }, 0, BLACK);
	}
	else
	{
		DrawCircle3D(position, radius, { 0, 0, 0 }, 0, GREEN);
	}
}


void BaseBuildingBlock::Scale()
{
	Vector2 mouseMove = GetMouseDelta() * changeSpeedMult;

	if (selectedAxis == Axis::X)
	{
		scale.x -= mouseMove.x;
		if (scale.x <= MIN_SCALE)
			scale.x = MIN_SCALE;
	}
	else if (selectedAxis == Axis::Y)
	{
		scale.y -= mouseMove.y;
		if (scale.y <= MIN_SCALE)
			scale.y = MIN_SCALE;
	}
	else if (selectedAxis == Axis::Z)
	{
		scale.z -= mouseMove.x;
		if (scale.z <= MIN_SCALE)
			scale.z = MIN_SCALE;
	}

	UpdateArrowBounds();
	UpdateTransformMatrix();

	UpdateArrowBounds();
	UpdateTransformMatrix();
}

void BaseBuildingBlock::ScaleUpdate()
{
	hoveredAxis = CheckArrowSelectedOn();

	if (hoveredAxis == Axis::None)
		return;

	Scale();
}

void BaseBuildingBlock::ScaleDraw()
{
	// X
	Vector3 endPosX = position;
	endPosX.x += arrowLength + (scale.x * 0.5f);
	Vector3 cubePosX = endPosX;
	cubePosX.x += arrowHeadBase / 2.0f;

	if (hoveredAxis == Axis::X)
	{
		DrawCylinderEx(position, endPosX, lineWidth, lineWidth, 10, BROWN);
		DrawCube(cubePosX, arrowHeadBase, arrowHeadBase, arrowHeadBase, BROWN);
	}
	else
	{
		DrawCylinderEx(position, endPosX, lineWidth, lineWidth, 10, RED);
		DrawCube(cubePosX, arrowHeadBase, arrowHeadBase, arrowHeadBase, RED);
	}

	// Y
	Vector3 endPosY = position;
	endPosY.y += arrowLength + (scale.y * 0.5f);
	Vector3 cubePosY = endPosY;
	cubePosY.y += arrowHeadBase / 2.0f;

	if (hoveredAxis == Axis::Y)
	{
		DrawCylinderEx(position, endPosY, lineWidth, lineWidth, 10, DARKBLUE);
		DrawCube(cubePosY, arrowHeadBase, arrowHeadBase, arrowHeadBase, DARKBLUE);
	}
	else
	{
		DrawCylinderEx(position, endPosY, lineWidth, lineWidth, 10, BLUE);
		DrawCube(cubePosY, arrowHeadBase, arrowHeadBase, arrowHeadBase, BLUE);
	}

	// Z
	Vector3 endPosZ = position;
	endPosZ.z += arrowLength + (scale.z * 0.5f);
	Vector3 cubePosZ = endPosZ;
	cubePosZ.z += arrowHeadBase / 2.0f;

	if (hoveredAxis == Axis::Z)
	{
		DrawCylinderEx(position, endPosZ, lineWidth, lineWidth, 10, DARKGREEN);
		DrawCube(cubePosZ, arrowHeadBase, arrowHeadBase, arrowHeadBase, DARKGREEN);
	}
	else
	{
		DrawCylinderEx(position, endPosZ, lineWidth, lineWidth, 10, GREEN);
		DrawCube(cubePosZ, arrowHeadBase, arrowHeadBase, arrowHeadBase, GREEN);
	}
}

void BaseBuildingBlock::UpdateBounds()
{
}

void BaseBuildingBlock::UpdateTransformMatrix()
{
	Matrix scaleMatrix = MatrixScale(scale.x, scale.y, scale.z);

	Matrix rotationMatrix = MatrixRotateXYZ({ rotation.x, rotation.y, rotation.z });

	Matrix translationMatrix = MatrixTranslate(position.x, position.y, position.z);

	transform = MatrixMultiply(
		MatrixMultiply(scaleMatrix, rotationMatrix),
		translationMatrix
	);

	model.transform = transform;
}

Axis BaseBuildingBlock::CheckArrowSelectedOn()
{
	int axis = -1; 

	Vector3 dir = Vector3Normalize(Vector3Subtract(CAMERA.target, CAMERA.position));
	Ray ray = { CAMERA.position, dir };

	float closestDist = 9999.0f;

	for (int i = 0; i < 3; i++)
	{
		RayCollision collision = GetRayCollisionBox(ray, boundingBoxes[i]);

		if (collision.hit && collision.distance < closestDist)
		{
			closestDist = collision.distance;
			axis = i;
		}
	}

	// Return -1 if none pressed
	return static_cast<Axis>(axis);
}

Axis BaseBuildingBlock::CheckCircleSelectedOn()
{
	Axis axis = Axis::None;

	float largestScale = std::max(scale.x, std::max(scale.y, scale.z));
	float radius = largestScale * ROTATION_CIRCLE_RADIUS_MULT;

	Vector3 dir = Vector3Normalize(Vector3Subtract(CAMERA.target, CAMERA.position));
	Ray ray = { CAMERA.position, dir };

	float closestDist = 9999.0f;

	// Quad points (X locked)
	Vector3 p1 = { position.x, position.y - radius, position.z - radius };
	Vector3 p2 = { position.x, position.y + radius, position.z - radius };
	Vector3 p3 = { position.x, position.y + radius, position.z + radius };
	Vector3 p4 = { position.x, position.y - radius, position.z + radius };

	RayCollision xCircleCollision = GetRayCollisionQuad(ray, p1, p2, p3, p4);

	if (xCircleCollision.hit && xCircleCollision.distance < closestDist)
	{
		axis = Axis::X;
		closestDist = xCircleCollision.distance;
	}

	// Quad points (Y locked)
	p1 = { position.x - radius, position.y, position.z - radius };
	p2 = { position.x + radius, position.y, position.z - radius };
	p3 = { position.x + radius, position.y, position.z + radius };
	p4 = { position.x - radius, position.y, position.z + radius };

	RayCollision yCircleCollision = GetRayCollisionQuad(ray, p1, p2, p3, p4);

	if (yCircleCollision.hit && yCircleCollision.distance < closestDist)
	{
		axis = Axis::Y;
		closestDist = yCircleCollision.distance;
	}

	// Quad points (Z locked)
	p1 = { position.x - radius, position.y - radius, position.z };
	p2 = { position.x + radius, position.y - radius, position.z };
	p3 = { position.x + radius, position.y + radius, position.z };
	p4 = { position.x - radius, position.y + radius, position.z };

	RayCollision zCircleCollision = GetRayCollisionQuad(ray, p1, p2, p3, p4);

	if (zCircleCollision.hit && zCircleCollision.distance < closestDist)
	{
		axis = Axis::Z;
		closestDist = zCircleCollision.distance;
	}

	// Return -1 if none pressed
	return axis;
}

void BaseBuildingBlock::UpdateArrowBounds()
{
	// X Bounds
	float totalLengthX = arrowLength + arrowHeadHeight + (scale.x * 0.5f);
	boundingBoxes[0].min = { position.x - arrowHeadBase, position.y - arrowHeadBase, position.z - arrowHeadBase};
	boundingBoxes[0].max = { position.x + totalLengthX, position.y + arrowHeadBase, position.z + arrowHeadBase };

	// Y Bounds
	float totalLengthY = arrowLength + arrowHeadHeight + (scale.y * 0.5f);
	boundingBoxes[1].min = { position.x - arrowHeadBase, position.y - arrowHeadBase, position.z - arrowHeadBase };
	boundingBoxes[1].max = { position.x + arrowHeadBase, position.y + totalLengthY, position.z + arrowHeadBase };

	// Z Bounds
	float totalLengthZ = arrowLength + arrowHeadHeight + (scale.z * 0.5f);
	boundingBoxes[2].min = { position.x - arrowHeadBase, position.y - arrowHeadBase, position.z - arrowHeadBase };
	boundingBoxes[2].max = {position.x + arrowHeadBase, position.y + arrowHeadBase, position.z + totalLengthZ };
}
