#pragma once
#include <raylib.h>
#include <memory>
#include "BaseBuildingBlock.h"

class StructureBuilding
{
public:
	StructureBuilding();

	void Init();
	void Draw();
	void Update();

private:
	void GetEditModeInputs();
	EditTypes editMode = EditTypes::Move;

	std::shared_ptr<BaseBuildingBlock> testObject;

	Camera3D camera;
};

