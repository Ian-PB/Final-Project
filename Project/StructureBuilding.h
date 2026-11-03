#pragma once
#include <raylib.h>
#include <memory>
#include <vector>
#include "BaseBuildingBlock.h"

class StructureBuilding
{
public:
	StructureBuilding();

	void Init();
	void Draw();
	void Update();

private:
	void CheckObjectClicked();

	void GetEditModeInputs();
	EditTypes editMode = EditTypes::Move;

	std::vector<std::shared_ptr<BaseBuildingBlock>> objects;
	std::shared_ptr<BaseBuildingBlock> selectedObject;

	Camera3D camera;

	bool isDragging = false;
};

