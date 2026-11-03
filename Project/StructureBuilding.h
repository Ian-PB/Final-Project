#pragma once
#include <raylib.h>
#include <memory>
#include <vector>
#include "BaseBuildingBlock.h"
#include "World.h"

class StructureBuilding
{
public:
	StructureBuilding();

	void Init();
	void Draw();
	void Update();

private:
	void SpawnObject();
	void CheckObjectClicked();

	void GetEditModeInputs();
	EditTypes editMode = EditTypes::Move;

	std::vector<std::shared_ptr<BaseBuildingBlock>> objects;
	std::shared_ptr<BaseBuildingBlock> selectedObject;

	Camera3D camera;

	bool isDragging = false;

	World world;
	Vector3 worldCenter;
	Vector3 worldSize;
};

