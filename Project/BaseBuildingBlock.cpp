#include "BaseBuildingBlock.h"
#include <raymath.h>
#include <algorithm>

BaseBuildingBlock::BaseBuildingBlock(const Camera& t_cam) : CAMERA(t_cam)
{
}

void BaseBuildingBlock::Init()
{
	UpdateArrowBounds();
}

void BaseBuildingBlock::Draw(EditTypes t_editType)
{
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

void BaseBuildingBlock::MoveDraw()
{
	// X
	Vector3 endPosX = position;
	endPosX.x += arrowLength;
	Vector3 pointStartPosX = endPosX;
	pointStartPosX.x += arrowHeadHeight;

	if (selectedAxis == Axis::X)
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
	endPosY.y += arrowLength;
	Vector3 pointStartPosY = endPosY;
	pointStartPosY.y += arrowHeadHeight;

	if (selectedAxis == Axis::Y)
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
	endPosZ.z += arrowLength;
	Vector3 pointStartPosZ = endPosZ;
	pointStartPosZ.z += arrowHeadHeight;

	if (selectedAxis == Axis::Z)
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

void BaseBuildingBlock::MoveUpdate()
{
	selectedAxis = CheckArrowSelectedOn();

	if (selectedAxis == Axis::None)
		return;
}

void BaseBuildingBlock::RotateDraw()
{
	// Draw 3 circle outlines (Up, Left, Right) encasing object
}

void BaseBuildingBlock::RotateUpdate()
{
}

void BaseBuildingBlock::ScaleDraw()
{
	// X
	Vector3 endPosX = position;
	endPosX.x += arrowLength;
	Vector3 cubePosX = endPosX;
	cubePosX.x += arrowHeadBase / 2.0f;

	if (selectedAxis == Axis::X)
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
	endPosY.y += arrowLength;
	Vector3 cubePosY = endPosY;
	cubePosY.y += arrowHeadBase / 2.0f;

	if (selectedAxis == Axis::Y)
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
	endPosZ.z += arrowLength;
	Vector3 cubePosZ = endPosZ;
	cubePosZ.z += arrowHeadBase / 2.0f;

	if (selectedAxis == Axis::Z)
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

void BaseBuildingBlock::ScaleUpdate()
{
	selectedAxis = CheckArrowSelectedOn();

	if (selectedAxis == Axis::None)
		return;
}

Axis BaseBuildingBlock::CheckArrowSelectedOn()
{
	int selectedAxis = -1; // If -1 returned then not on any

	Vector3 dir = Vector3Normalize(Vector3Subtract(CAMERA.target, CAMERA.position));
	Ray ray = { CAMERA.position, dir };

	float closestDist = 9999.0f;

	for (int i = 0; i < 3; i++)
	{
		RayCollision collision = GetRayCollisionBox(ray, boundingBoxes[i]);

		if (collision.hit && collision.distance < closestDist)
		{
			closestDist = collision.distance;
			selectedAxis = i;
		}
	}

	// Return -1 if none pressed
	return static_cast<Axis>(selectedAxis);
}

void BaseBuildingBlock::UpdateArrowBounds()
{
	float totalLength = arrowLength + arrowHeadHeight;

	// X Bounds
	boundingBoxes[0].min = { position.x - arrowHeadBase, position.y - arrowHeadBase, position.z - arrowHeadBase};
	boundingBoxes[0].max = { position.x + totalLength, position.y + arrowHeadBase, position.z + arrowHeadBase };

	// Y Bounds
	boundingBoxes[1].min = { position.x - arrowHeadBase, position.y - arrowHeadBase, position.z - arrowHeadBase };
	boundingBoxes[1].max = { position.x + arrowHeadBase, position.y + totalLength, position.z + arrowHeadBase };

	// Z Bounds
	boundingBoxes[2].min = { position.x - arrowHeadBase, position.y - arrowHeadBase, position.z - arrowHeadBase };
	boundingBoxes[2].max = {position.x + arrowHeadBase, position.y + arrowHeadBase, position.z + totalLength };
}
